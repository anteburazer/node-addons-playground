#include <node.h>
#include <chrono>
#include <thread>
#include "rainfall.h" 

using namespace v8;

location _unpack_location(Isolate * isolate, const Local<Object> location_obj);
sample _unpack_sample(Isolate * isolate, const Local<Object> sample);
void _pack_rain_result(Isolate* isolate, Local<Object> & target, rain_result & result);
static void _workAsync(uv_work_t *req);
static void _workAsyncComplete(uv_work_t *req, int status);

// -------------------------------------- Public methods --------------------------------------
void AvgRainfall(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> location_obj = args[0]->ToObject(context).ToLocalChecked();

    location loc = _unpack_location(isolate, location_obj);
    double avg = calculate_avg_rainfall(loc);
    
    Local<Number> retval = Number::New(isolate, avg);
    args.GetReturnValue().Set(retval);
}

void RainfallData(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> location_obj = args[0]->ToObject(context).ToLocalChecked();

    location loc = _unpack_location(isolate, location_obj);
    rain_result result = calculate_rainfall_statistics(loc);

    Local<Object> rainfallData = Object::New(isolate);

    rainfallData->Set(
        String::NewFromUtf8(isolate, "mean"), 
        Number::New(isolate, result.mean)
    );

    rainfallData->Set(
        String::NewFromUtf8(isolate, "median"), 
        Number::New(isolate, result.median)
    );

    rainfallData->Set(
        String::NewFromUtf8(isolate, "standard_deviation"), 
        Number::New(isolate, result.standard_deviation)
    );

    rainfallData->Set(
        String::NewFromUtf8(isolate, "n"), 
        Number::New(isolate, result.n)
    );

    args.GetReturnValue().Set(rainfallData);
}

void CalculateResults(const v8::FunctionCallbackInfo<v8::Value>&args) {
    Isolate* isolate = args.GetIsolate();

    // TODO: What is vector?
    std::vector<location> locations;
    std::vector<rain_result> results;

    // extract each location (its a list)
    Local<Array> input = Local<Array>::Cast(args[0]);
    unsigned int num_locations = input->Length();

    for (unsigned int i = 0; i < num_locations; i++) {
        locations.push_back(
            _unpack_location(isolate, Local<Object>::Cast(input->Get(i)))
        );
    }
    
    // Build vector of rain_results
    // TODO: What is going on here???
    results.resize(locations.size());
    std::transform(
        locations.begin(), 
        locations.end(), 
        results.begin(), 
        calculate_rainfall_statistics
    );

    // Convert the rain_results into Objects for return
    Local<Array> result_list = Array::New(isolate);

    for (unsigned int i = 0; i < results.size(); i++ ) {
        Local<Object> result = Object::New(isolate);
        _pack_rain_result(isolate, result, results[i]);
        result_list->Set(i, result);
    }

    args.GetReturnValue().Set(result_list);
}

void CalculateResultsSyncWithCallback(const v8::FunctionCallbackInfo<v8::Value>&args) {
    Isolate* isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    // TODO: What is vector?
    std::vector<location> locations;
    std::vector<rain_result> results;

    // extract each location (its a list)
    Local<Array> input = Local<Array>::Cast(args[0]);
    unsigned int num_locations = input->Length();

    for (unsigned int i = 0; i < num_locations; i++) {
        locations.push_back(
            _unpack_location(isolate, Local<Object>::Cast(input->Get(i)))
        );
    }
    
    // Build vector of rain_results
    // TODO: What is going on here???
    results.resize(locations.size());
    std::transform(
        locations.begin(), 
        locations.end(), 
        results.begin(), 
        calculate_rainfall_statistics
    );

    // Convert the rain_results into Objects for return
    Local<Array> result_list = Array::New(isolate);

    for (unsigned int i = 0; i < results.size(); i++ ) {
        Local<Object> result = Object::New(isolate);
        _pack_rain_result(isolate, result, results[i]);
        result_list->Set(i, result);
    }

    Local<Function> callback = Local<Function>::Cast(args[1]);
    Local<Value> argv[] = { result_list };
    callback->Call(context, isolate->GetCurrentContext()->Global(), 1, argv);

    args.GetReturnValue().Set(Undefined(isolate));
}

void CalculateResultsAsync(const v8::FunctionCallbackInfo<v8::Value>&args) {
    Isolate* isolate = args.GetIsolate();

    Work *work = new Work();
    work->request.data = work;

    // extract each location (its a list) and store it in the work package
    // work is on the heap, accessible in the libuv threads
    Local<Array> input = Local<Array>::Cast(args[0]);
    unsigned int num_locations = input->Length();

    for (unsigned int i = 0; i < num_locations; i++) {
        work->locations.push_back(
            _unpack_location(isolate, Local<Object>::Cast(input->Get(i)))
        );
    }

    // store the callback from JS in the work package so we can invoke it later
    Local<Function> callback = Local<Function>::Cast(args[1]);
    work->callback.Reset(isolate, callback);

    // kick of the worker thread
    uv_queue_work(uv_default_loop(), &work->request, _workAsync, _workAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
}

// -------------------------------------- Private methods --------------------------------------

// called by libuv worker in separate thread
static void _workAsync(uv_work_t *req) {
    Work *work = static_cast<Work *>(req->data);

    // this is the worker thread, lets build up the results allocated results from the heap because we'll need 
    // to access in the event loop later to send back
    work->results.resize(work->locations.size());
    std::transform(
        work->locations.begin(), 
        work->locations.end(), 
        work->results.begin(), 
        calculate_rainfall_statistics
    );

    // that wasn't really that long of an operation, so lets pretend it took longer...
    std::this_thread::sleep_for(chrono::seconds(3));
}

// // called by libuv in event loop when async function completes
static void _workAsyncComplete(uv_work_t *req, int status) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope handle_scope(isolate);

    Local<Context> context = isolate->GetCurrentContext();


    Local<Array> result_list = Array::New(isolate);
    Work *work = static_cast<Work *>(req->data);

    // the work has been done, and now we pack the results
    // vector into a Local array on the event-thread's stack.
    for (unsigned int i = 0; i < work->results.size(); i++ ) {
        Local<Object> result = Object::New(isolate);
        _pack_rain_result(isolate, result, work->results[i]);
        result_list->Set(i, result);
    }

    Local<Value> argv[] = { Null(isolate) , result_list };

    Local<Function>::New(isolate, work->callback)->Call(context, context->Global(), 2, argv);

    // Free up the persistent function callback
    work->callback.Reset();
    delete work;
}

location _unpack_location(Isolate * isolate, Local<Object> location_obj) {
    location loc;
    Local<Context> context = isolate->GetCurrentContext();

    Local<Value> latitude = location_obj->Get(String::NewFromUtf8(isolate, "latitude"));
    Local<Value> longitude = location_obj->Get(String::NewFromUtf8(isolate, "longitude"));
    Local<Array> samples = Local<Array>::Cast(
        location_obj->Get(String::NewFromUtf8(isolate, "samples"))
    );

    loc.latitude = latitude->NumberValue(context).FromMaybe(0);
    loc.longitude = longitude->NumberValue(context).FromMaybe(0);

    int samples_count = samples->Length();

    for (int i = 0; i < samples_count; i++) {
        sample s = _unpack_sample(isolate, Local<Object>::Cast(samples->Get(i)));
        loc.samples.push_back(s);
    }

    return loc;
}

sample _unpack_sample(Isolate * isolate, const Local<Object> sample_obj) {
    sample sem;

    Local<Context> context = isolate->GetCurrentContext();

    Local<Value> date_value = sample_obj->Get(String::NewFromUtf8(isolate, "date"));
    Local<Value> rainfall_value = sample_obj->Get(String::NewFromUtf8(isolate, "rainfall"));
    
    // TODO: How utfValue works?
    String::Utf8Value utfValue(isolate, date_value);

    // TODO: What 'std::string(*utfValue)' means?
    sem.date = std::string(*utfValue);
    sem.rainfall = rainfall_value->NumberValue(context).FromMaybe(0);

    return sem;
}

void _pack_rain_result(v8::Isolate* isolate, v8::Local<v8::Object> & target, rain_result & result){
    target->Set(String::NewFromUtf8(isolate, "mean"), Number::New(isolate, result.mean));
    target->Set(String::NewFromUtf8(isolate, "median"), Number::New(isolate, result.median));
    target->Set(String::NewFromUtf8(isolate, "standard_deviation"), Number::New(isolate, result.standard_deviation));
    target->Set(String::NewFromUtf8(isolate, "n"), Integer::New(isolate, result.n));
}

void Init(Local<Object> exports, Local<Object> module) {
    NODE_SET_METHOD(exports, "avg_rainfall", AvgRainfall);
    NODE_SET_METHOD(exports, "rainfall_data", RainfallData);
    NODE_SET_METHOD(exports, "calculate_results", CalculateResults);
    NODE_SET_METHOD(exports, "calculate_results_sync_with_callback", CalculateResultsSyncWithCallback);
    NODE_SET_METHOD(exports, "calculate_results_async", CalculateResultsAsync); 
}

NODE_MODULE(rainfall, Init)