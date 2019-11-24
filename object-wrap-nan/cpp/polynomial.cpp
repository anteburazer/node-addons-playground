#include <cmath>
#include <nan.h>
#include <string>
using namespace Nan;

static Persistent<v8::FunctionTemplate> constructor;

class WrappedPoly : public Nan::ObjectWrap {
    public:
        static NAN_MODULE_INIT(Init) {
            // Prepare constructor template
            v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(WrappedPoly::New);

            tpl->SetClassName(
                Nan::New<v8::String>("Polynomial").ToLocalChecked()
            );
            tpl->InstanceTemplate()->SetInternalFieldCount(1);

            // Prototype
            SetPrototypeMethod(tpl, "at", WrappedPoly::At);
            SetPrototypeMethod(tpl, "roots", WrappedPoly::Roots);

            v8::Local<v8::ObjectTemplate> itpl = tpl->InstanceTemplate();

            SetAccessor(
                itpl, 
                Nan::New<v8::String>("a").ToLocalChecked(), 
                WrappedPoly::GetCoeff, WrappedPoly::SetCoeff
            );

            SetAccessor(
                itpl,
                Nan::New<v8::String>("b").ToLocalChecked(), 
                WrappedPoly::GetCoeff, WrappedPoly::SetCoeff
            );

            SetAccessor(itpl, 
                Nan::New<v8::String>("c").ToLocalChecked(), 
                WrappedPoly::GetCoeff, WrappedPoly::SetCoeff
            );

            constructor.Reset(tpl);
            Set(
                target, 
                Nan::New<v8::String>("Polynomial").ToLocalChecked(), 
                Nan::GetFunction(tpl).ToLocalChecked()
            );
        }

    private:
        explicit WrappedPoly(double a = 0, double b = 0, double c = 0)
            : a_(a), b_(b), c_(c) {}
        ~WrappedPoly() {}

        static NAN_METHOD(New) {
            v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();

            double a = info[0]->IsUndefined() ? 0 : info[0]->NumberValue(context).FromJust();
            double b = info[1]->IsUndefined() ? 0 : info[1]->NumberValue(context).FromJust();
            double c = info[2]->IsUndefined() ? 0 : info[2]->NumberValue(context).FromJust();

            WrappedPoly* obj = new WrappedPoly(a, b, c);
            obj->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
        };

        static NAN_METHOD(At);
        static NAN_METHOD(Roots);

        static NAN_GETTER(GetCoeff);
        static NAN_SETTER(SetCoeff);

        double a_, b_, c_;
};

NAN_METHOD(WrappedPoly::At){
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();

    double x = info[0]->IsUndefined() ? 0 : info[0]->NumberValue(context).FromJust();
    WrappedPoly* poly = ObjectWrap::Unwrap<WrappedPoly>(info.Holder());

    double results = (x * x * poly->a_) + (x * poly->b_) + poly->c_;

    info.GetReturnValue().Set(
        Nan::New<v8::Number>(results)
    );
}

NAN_METHOD(WrappedPoly::Roots){
    WrappedPoly* poly = ObjectWrap::Unwrap<WrappedPoly>(info.Holder());
    v8::Local<v8::Array> roots = Nan::New<v8::Array>();
    double desc = poly->b_ * poly->b_ - (4 * poly->a_ * poly->c_);

    if (desc >= 0 ) {
        double r = (-poly->b_ + sqrt(desc)) / (2 * poly->a_);
        roots->Set(0,Nan::New<v8::Number>(r));

        if (desc > 0) {
            r = (-poly->b_ - sqrt(desc)) / (2 * poly->a_);
            roots->Set(1,Nan::New<v8::Number>(r));
        }
    }

    info.GetReturnValue().Set(roots);
}

NAN_GETTER(WrappedPoly::GetCoeff) {
    v8::Isolate* isolate = info.GetIsolate();
    WrappedPoly* obj = ObjectWrap::Unwrap<WrappedPoly>(info.This());

    Nan::Utf8String s(property);
    std::string str (*s);

    if ( str == "a") {
        info.GetReturnValue().Set(
            v8::Number::New(isolate, obj->a_)
        );
    } else if (str == "b") {
        info.GetReturnValue().Set(
            v8::Number::New(isolate, obj->b_)
        );
    } else if (str == "c") {
        info.GetReturnValue().Set(
            v8::Number::New(isolate, obj->c_)
        );
    }
}

NAN_SETTER(WrappedPoly::SetCoeff) {
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
    WrappedPoly* obj = ObjectWrap::Unwrap<WrappedPoly>(info.This());

    Nan::Utf8String s(property);
    std::string str (*s);

    if ( str == "a") obj->a_ = value->NumberValue(context).FromJust();
    else if (str == "b") obj->b_ = value->NumberValue(context).FromJust();
    else if (str == "c") obj->c_ = value->NumberValue(context).FromJust();
}

void InitPoly(v8::Local<v8::Object> exports) {
    WrappedPoly::Init(exports);
}

NODE_MODULE(polynomial, InitPoly)