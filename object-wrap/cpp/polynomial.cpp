#include <node.h>
#include <iostream>
#include <cmath>
#include <node_object_wrap.h>

using namespace std;
using namespace v8;

class WrappedPoly : public node::ObjectWrap {
    public:
        static void Init(Local<Object> exports) {
            Isolate* isolate = exports->GetIsolate();
            Local<Context> context = isolate->GetCurrentContext();

            // Prepare constructor template
            Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);

            tpl->SetClassName(
                String::NewFromUtf8(isolate, "Polynomial")
            );

            tpl->InstanceTemplate()->SetInternalFieldCount(1);

            // Add the "at" and "roots" function to the object prototype
            NODE_SET_PROTOTYPE_METHOD(tpl, "at", At);
            NODE_SET_PROTOTYPE_METHOD(tpl, "roots", Roots);

            // Add an accessor - get/set
            tpl->InstanceTemplate()->SetAccessor(
                String::NewFromUtf8(isolate, "a"), GetCoeff, SetCoeff
            );
            tpl->InstanceTemplate()->SetAccessor(
                String::NewFromUtf8(isolate, "b"), GetCoeff, SetCoeff
            );
            tpl->InstanceTemplate()->SetAccessor(
                String::NewFromUtf8(isolate, "c"), GetCoeff, SetCoeff
            );

            // Store a reference to this constructor so it can be called in New if New is called without constructor syntax.
            constructor.Reset(
                isolate, 
                tpl->GetFunction(context).ToLocalChecked()
            );

            exports->Set(
                String::NewFromUtf8(isolate, "Polynomial"),
                tpl->GetFunction(context).ToLocalChecked()
            );
        }

    private:
        explicit WrappedPoly(double a = 0, double b = 0, double c = 0) 
            : a_(a), b_(b), c_(c) {};
        ~WrappedPoly() {}

        static void New(const FunctionCallbackInfo<Value> &args) {
            Isolate* isolate = args.GetIsolate();
            Local<Context> context = isolate->GetCurrentContext();
            
            if (args.IsConstructCall()) {
                double a = args[0]->IsUndefined() ? 0 : args[0]->NumberValue(context).FromMaybe(0);
                double b = args[1]->IsUndefined() ? 0 : args[1]->NumberValue(context).FromMaybe(0);
                double c = args[2]->IsUndefined() ? 0 : args[2]->NumberValue(context).FromMaybe(0);

                WrappedPoly* obj = new WrappedPoly(a, b, c);
                obj->Wrap(args.This());
                args.GetReturnValue().Set(args.This());
            } else {
                // Invoked as plain function `Polynomial(...)`, turn into construct call.
                const int argc = 3;
                Local<Value> argv[argc] = { args[0] , args[1], args[2] };
                Local<Function> cons = Local<Function>::New(isolate, constructor);

                args.GetReturnValue().Set(
                    cons->NewInstance(context, argc, argv).ToLocalChecked()
                );
            }
        }

        static void At(const FunctionCallbackInfo<Value> &args);
        static void Roots(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void GetCoeff(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void SetCoeff(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);

        static Persistent<Function> constructor;
        double a_;
        double b_;
        double c_;
};

void WrappedPoly::At(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    double x = args[0]->IsUndefined() ? 0 : args[0]->NumberValue(context).FromMaybe(0);
    WrappedPoly* poly = ObjectWrap::Unwrap<WrappedPoly>(args.Holder());

    double results =  x * x * poly->a_ + x * poly->b_ + poly->c_;

    args.GetReturnValue().Set(Number::New(isolate, results));
}

void WrappedPoly::Roots(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    WrappedPoly* poly = ObjectWrap::Unwrap<WrappedPoly>(args.Holder());

    Local<Array> roots = Array::New(isolate);
    double desc = poly->b_ * poly->b_ - (4 * poly->a_ * poly->c_);

    if (desc >= 0 ) {
        double r = (-poly->b_ + sqrt(desc))/(2 * poly->a_);
        roots->Set(0,Number::New(isolate, r));
        if ( desc > 0) {
            r = (-poly->b_ - sqrt(desc))/(2 * poly->a_);
            roots->Set(1,Number::New(isolate, r));
        }
    }

    args.GetReturnValue().Set(roots);
}

void WrappedPoly::GetCoeff(Local<String> property, const PropertyCallbackInfo<Value>& info) {
    Isolate* isolate = info.GetIsolate();
    WrappedPoly* obj = ObjectWrap::Unwrap<WrappedPoly>(info.This());
    
    String::Utf8Value s(isolate, property);
    std::string str(*s);

    if (str == "a") {
        info.GetReturnValue().Set(Number::New(isolate, obj->a_));
    } else if (str == "b") {
        info.GetReturnValue().Set(Number::New(isolate, obj->b_));
    } else if (str == "c") {
        info.GetReturnValue().Set(Number::New(isolate, obj->c_));
    }
}

void WrappedPoly::SetCoeff(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info) {
    Isolate* isolate = info.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    WrappedPoly* obj = ObjectWrap::Unwrap<WrappedPoly>(info.This());

    String::Utf8Value s(isolate, property);
    std::string str(*s);

    if (str == "a") {
        obj->a_ = value->NumberValue(context).FromMaybe(0);
    } else if (str == "b") {
        obj->b_ = value->NumberValue(context).FromMaybe(0);
    } else if (str == "c") {
        obj->c_ = value->NumberValue(context).FromMaybe(0);
    }
}

Persistent<Function> WrappedPoly::constructor;

void InitPoly(Local<Object> exports) {
    WrappedPoly::Init(exports);
}

NODE_MODULE(polynomial, InitPoly)