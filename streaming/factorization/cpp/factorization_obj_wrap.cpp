#include <nan.h>
#include <chrono>
#include <thread>
#include "streaming-worker.h"

using namespace Nan;

/*
 * Responsabilities:
 * 
 * In streaming-worker.h - intended to be reused in each addon
 *  1. Implementation of PQueue, Message classes
 *  2. Implementation of StreamingWorker abstract class - each addon will extend this and implement a constructor and Execute method
 *  3. Implementation of StreamingWorkerWrapper which will be exposed to JavaScript by each addon
 * 
 * 
 * In an addon's source code, we'd include streaming-worker.h and then...
 *  1. Extend StreamingWorker
 *  2. Provide an implementation of create_worker which StreamingWorkerWrapper will call
 *  3. NODE_MODULE(<addon name>, StreamWorkerWrapper::Init) to expose the wrapper to Node.js. Note, each addon will have a different name.
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

        void send_factor(const AsyncProgressWorker::ExecutionProgress& progressCb, long long factor) {
            Message msg("factor", std::to_string(factor));
            writeToNode(progressCb, msg);
        }

    private:
        long long N;
};

StreamingWorker *create_worker(
    Callback *progressCallback,
    Callback *completeCallback,
    Callback *errorCallback,
    v8::Local<v8::Object> & options) {
    return new Factorizer(progressCallback, completeCallback, errorCallback, options);
}

NODE_MODULE(factorization, StreamWorkerWrapper::Init)