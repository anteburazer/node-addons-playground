#include <node.h>

using namespace v8;

void Add(const FunctionCallbackInfo<Value> & args) {
    Isolate * isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    if (args.Length() < 2) {
        // Throw an Error that is passed back to JavaScript 
        isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, "Wrong number of arguments", NewStringType::kNormal).ToLocalChecked()
        ));
        return;
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber()) { 
        return; // undefined goes back to the caller
    }

    // value is created on the C++ runtime stack, not as part of
    // the JavaScript execution context...
    double value = args[0]->NumberValue(context).FromMaybe(0) + args[1]->NumberValue(context).FromMaybe(0);

    // Now we create it in the JS execution context so we can return it
    Local<Number> num = Number::New(isolate, value);
    args.GetReturnValue().Set(num);
}

void Init(Local<Object> exports) {
    NODE_SET_METHOD(exports, "add", Add);
}

NODE_MODULE(cmath_addon, Init);