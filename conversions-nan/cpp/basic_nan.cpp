#include <nan.h>
#include <iostream>
#include <string>
using namespace std;
using namespace Nan;
using namespace v8;

NAN_METHOD(PassNumber) {
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();

    if (info.Length() < 1) {
        info.GetReturnValue().Set(Nan::New(0));
        return;
    }

    if (!info[0]->IsNumber()) {
        info.GetReturnValue().Set(Nan::New(-1));
        return;
    }

    /*
     * Or:
     * 
     * // FromJust() is used to get get the actual double value
     * double value = info[0]->NumberValue(context).FromJust();
     * Local<Number> retval = Nan::New(value + 42);
     * 
     * Or:
     * 
     * Nan::MaybeLocal<Number> value = Nan::To<Number>(info[0]);
     * Local<Number> checked = value.ToLocalChecked();
     * Local<Number> retval = Nan::New(checked->Value() + 42);
     */

    Nan::Maybe<double> value = Nan::To<double>(info[0]);
    Local<Number> retval = Nan::New(value.FromJust() + 42);

    info.GetReturnValue().Set(retval);
}

NAN_METHOD(PassInteger) {
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();

    if (info.Length() < 1) {
        return;
    }

    if (!info[0]->IsInt32()) {
        return;
    }

    int value = info[0]->IntegerValue(context).FromJust();
    Local<Integer> retval = Nan::New(value + 42);
    info.GetReturnValue().Set(retval); 
}

NAN_METHOD(PassBoolean) {
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();

    if (info.Length() < 1) {
        return;
    }
    if (!info[0]->IsBoolean()) {
        return;
    }

    bool value = info[0]->BooleanValue(context).FromJust();
    Local<Boolean> retval = Nan::New(!value);
    info.GetReturnValue().Set(retval); 
}

NAN_METHOD(PassString) {
    // Nan::New returns Local for most JavaScript types, 
    // but it returns MaybeLocal when creating strings, dates, regular expressions, and script objects
    Nan::MaybeLocal<String> tmp = Nan::To<String>(info[0]);
    Local<String> local_string = tmp.ToLocalChecked();

    Nan::Utf8String val(local_string);

    std::string str (*val);
    std::reverse(str.begin(), str.end());

    Local<String> retval = Nan::New<String>(str.c_str()).ToLocalChecked();
    info.GetReturnValue().Set(retval); 
}

Local<Value> make_return(const Local<Object> input, const v8::Local<v8::Context> context) {
    Local<String> x_prop = Nan::New<String>("x").ToLocalChecked();
    Local<String> y_prop = Nan::New<String>("y").ToLocalChecked();
    Local<String> sum_prop = Nan::New<String>("sum").ToLocalChecked();
    Local<String> product_prop = Nan::New<String>("product").ToLocalChecked();

    Local<Object> obj = Nan::New<Object>();
    double x = Nan::Get(input, x_prop).ToLocalChecked()->NumberValue(context).FromJust();
    double y = Nan::Get(input, y_prop).ToLocalChecked()->NumberValue(context).FromJust();
      
    Nan::Set(obj, sum_prop, Nan::New<Number>(x+y));
    Nan::Set(obj, product_prop, Nan::New<Number>(x*y));
    return obj;
}

NAN_METHOD(PassObject) {
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();

    if (info.Length() > 0) {
        Nan::MaybeLocal<Object> tmp = Nan::To<Object>(info[0]);
        Local<Object> local_object = tmp.ToLocalChecked();

        Local<Value> obj = make_return(local_object, context);
        info.GetReturnValue().Set(obj);
    }
}

NAN_METHOD(PassArray) {
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
    Local<Array> array = Local<Array>::Cast(info[0]);

    for (unsigned int i = 0; i < array->Length(); i++) {
        if (Nan::Has(array, i).FromJust()) {
            double value = Nan::Get(array, i).ToLocalChecked()->NumberValue(context).FromJust();
            Nan::Set(array, i, Nan::New<Number>(value + 1));
        }
    }

    Local<String> prop = Nan::New<String>("not_index").ToLocalChecked();
    Local<Array> a = New<v8::Array>(3);

    Nan::Set(a, 0, Nan::Get(array, 0).ToLocalChecked());
    Nan::Set(a, 1, Nan::Get(array, 1).ToLocalChecked());
    Nan::Set(a, 2, Nan::Get(array, prop).ToLocalChecked());

    info.GetReturnValue().Set(a);
}

NAN_MODULE_INIT(Init) {
    Nan::Set(
        target, 
        New<String>("passNumber").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(PassNumber)).ToLocalChecked()
    );

    Nan::Set(
        target, 
        New<String>("passInteger").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(PassInteger)).ToLocalChecked()
    );

    Nan::Set(
        target, 
        New<String>("passBoolean").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(PassBoolean)).ToLocalChecked()
    );

    Nan::Set(
        target, 
        New<String>("passString").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(PassString)).ToLocalChecked()
    );

    Nan::Set(
        target, 
        New<String>("passObject").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(PassObject)).ToLocalChecked()
    );

    Nan::Set(
        target, 
        New<String>("passArray").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(PassArray)).ToLocalChecked()
    );
}

NODE_MODULE(basic_nan, Init)