#include <node.h>

using namespace v8;

void Increment(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> target = args[0]->ToObject(context).ToLocalChecked();

    Local<String> x_prop = String::NewFromUtf8(isolate, "x");
    double x = target->Get(x_prop)->NumberValue(context).FromMaybe(0);

    target->Set(x_prop, Number::New(isolate, (x + 1)));
}

void Init(Local<Object> exports) {
    NODE_SET_METHOD(exports, "increment", Increment);
}

NODE_MODULE(object_manipulation, Init)