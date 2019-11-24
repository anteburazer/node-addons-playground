#include <nan.h>
#include <chrono>
#include <thread>
#include "streaming-worker.h"

using namespace Nan;

/*
 * 9007199254740991
 */

class Factorizer : public StreamingWorker {
    public:
        Factorizer(Callback *progressCb, Callback *completeCb, Callback *errorCb, v8::Local<v8::Object> &options)
            : StreamingWorker(progressCb, completeCb, errorCb) {
                N = -1;

                if (options->IsObject()) {
                    v8::Local<v8::Value> n_ = options->Get(New<v8::String>("n").ToLocalChecked());

                    if (n_->IsNumber()) {
                        N = n_->NumberValue(GetCurrentContext()).FromJust();
                    }
                }

                if (N < 0) {
                    SetErrorMessage("Cannot compute prime factorization of negative numbers (overflowed long long?)!");
                }
            }

        // Executes in the new worker thread (background)
        void Execute (const AsyncProgressWorker::ExecutionProgress& progressCb) {
            long long n = N;

            while (n%2 == 0) {
                send_factor(progressCb, 2);
                n = n/2;
            }

            for (long long i = 3; i <= n; i = i+2) {
                while (n%i == 0) {
                    send_factor(progressCb, i);
                    n = n/i;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        void send_factor(const AsyncProgressWorker::ExecutionProgress& progressCb, uint32_t factor) {
            Message msg("factor", std::to_string(factor));
            writeToNode(progressCb, msg);
        }

    private:
        long long N;
};

NAN_METHOD(Factor) {
    Nan::MaybeLocal<v8::Object> tmp = Nan::To<v8::Object>(info[0]);
    v8::Local<v8::Object> options = tmp.ToLocalChecked();

    Callback *completeCb = new Callback(info[1].As<v8::Function>());
    Callback *progressCb = new Callback(info[2].As<v8::Function>());
    Callback *errorCb = new Callback(info[3].As<v8::Function>());

    AsyncQueueWorker(
        new Factorizer(progressCb, completeCb, errorCb, options)
    );
}

NAN_MODULE_INIT(Init) {
    v8::Local<v8::FunctionTemplate> tpl = New<v8::FunctionTemplate>(Factor);

    Set(
        target,
        New<v8::String>("factorize").ToLocalChecked(),
        GetFunction(tpl).ToLocalChecked()
    );
}

NODE_MODULE(factorization, Init)