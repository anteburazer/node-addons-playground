#include <nan.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;
using namespace Nan;
using namespace v8;

/*
 * C++ implementation for calculating the prime numbers
 */
void find_primes(int limit, vector<int> & primes) {
    std::vector<int> is_prime(limit, true);

    for (int n = 2; n < limit; n++ ) {
        if (is_prime[n]) {
            primes.push_back(n);
        }

        for (int i = n * n; i < limit; i+= n) {
            is_prime[i] = false;
        }
    }
}

/*
 * Worker class with progress
 */
class PrimeProgressWorker : public AsyncProgressWorker {
    public:
        PrimeProgressWorker(Callback * callback, Callback * progress, int limit) 
            : AsyncProgressWorker(callback), progress(progress), limit(limit) {

        }
        
        // Executes in worker thread
        void Execute(const AsyncProgressWorker::ExecutionProgress & progress) {
            std::vector<int> is_prime(limit, true);

            for (int n = 2; n < limit; n++ ) {
                double p = (100.0 * n) / limit;
                progress.Send(reinterpret_cast<const char*>(&p), sizeof(double));

                if (is_prime[n] ) primes.push_back(n);

                for (int i = n * n; i < limit; i+= n) {
                    is_prime[i] = false;
                }

                std::this_thread::sleep_for(chrono::milliseconds(100));
            }
        }

        // Executes in event loop
        void HandleOKCallback () {
            Local<Array> results = New<Array>(primes.size());

            for ( unsigned int i = 0; i < primes.size(); i++ ) {
                Nan::Set(results, i, New<v8::Number>(primes[i]));
            }  

            Local<Value> argv[] = { results };
            callback->Call(1, argv);
        }

        void HandleProgressCallback(const char *data, size_t size) {
            // Required, this is not created automatically 
            Nan::HandleScope scope; 

            Local<Value> argv[] = {
                New<v8::Number>(*reinterpret_cast<double*>(const_cast<char*>(data)))
            };
            progress->Call(1, argv);
        }

    private:
        Callback *progress;
        int limit;
        vector<int> primes;
};

/*
 * Worker class without progress
 */
class PrimeWorker : public AsyncWorker {
    public:
        PrimeWorker(Callback * callback, int limit) : AsyncWorker(callback), limit(limit) {

        }
        
        // Executes in worker thread
        void Execute() {
            find_primes(limit, primes);
        }

        // Executes in event loop
        void HandleOKCallback () {
            Local<Array> results = New<Array>(primes.size());

            for ( unsigned int i = 0; i < primes.size(); i++ ) {
                Nan::Set(results, i, New<v8::Number>(primes[i]));
            }  

            Local<Value> argv[] = { results };
            callback->Call(1, argv);
        }

    private:
        int limit;
        vector<int> primes;
};

/*
 * Synchronous
 */
NAN_METHOD(PrimesSync) {
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();

    int limit = info[0]->IntegerValue(context).FromJust();
    Callback *callback = new Callback(info[1].As<Function>());

    vector<int> primes;
    find_primes(limit, primes);

    Local<Array> results = New<Array>(primes.size());

    for (unsigned int i = 0; i < primes.size(); i++) {
        Nan::Set(results, i, New<v8::Number>(primes[i]));
    }

    Local<Value> argv[] = { results };
    callback->Call(1, argv);
}

/*
 * Asynchronous
 */
NAN_METHOD(PrimesAsync) {
    int limit = To<int>(info[0]).FromJust();
    Callback *callback = new Callback(info[1].As<Function>());

    AsyncQueueWorker(new PrimeWorker(callback, limit));
}

/*
 * Asynchronous with progress
 */
NAN_METHOD(PrimesAsyncProgress) {
    int limit = To<int>(info[0]).FromJust();
    Callback *callback = new Callback(info[1].As<Function>());
    Callback *progress = new Callback(info[2].As<v8::Function>());

    AsyncQueueWorker(new PrimeProgressWorker(callback, progress, limit));
}

NAN_MODULE_INIT(Init) {
    Nan::Set(
        target, 
        New<String>("primesSync").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(PrimesSync)).ToLocalChecked()
    );

    Nan::Set(
        target, 
        New<String>("primesAsync").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(PrimesAsync)).ToLocalChecked()
    );

    Nan::Set(
        target, 
        New<String>("primesAsyncProgress").ToLocalChecked(),
        GetFunction(New<FunctionTemplate>(PrimesAsyncProgress)).ToLocalChecked()
    );
}

NODE_MODULE(primes, Init)