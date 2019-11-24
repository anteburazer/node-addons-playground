Nan::Persistent<v8::Function> WrappedPoly::constructor;

WrappedPoly::WrappedPoly(double a = 0, double b = 0, double c = 0) : a_(a), b_(b), c_(c) {}
WrappedPoly::~WrappedPoly() {}

void WrappedPoly::Init(v8::Local<v8::Object> exports) {
    v8::Local<v8::Context> context = exports->CreationContext();
    Nan::HandleScope scope;

    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(
        Nan::New<v8::String>("Polynomial").ToLocalChecked()
    );
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    // SetPrototypeMethod(tpl, "at", At);
    // SetPrototypeMethod(tpl, "roots", Roots);

    // v8::Local<v8::ObjectTemplate> itpl = tpl->InstanceTemplate();

    // SetAccessor(itpl, Nan::New<v8::String>("a").ToLocalChecked(), GetCoeff, SetCoeff);
    // SetAccessor(itpl, Nan::New<v8::String>("b").ToLocalChecked(), GetCoeff, SetCoeff);
    // SetAccessor(itpl, Nan::New<v8::String>("c").ToLocalChecked(), GetCoeff, SetCoeff);

    constructor.Reset(tpl);
    Set(
        target, 
        Nan::New<v8::String>("Polynomial").ToLocalChecked(), 
        tpl->GetFunction(context).ToLocalChecked()
    );
}

void WrappedPoly::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();

    if (info.IsConstructCall()) {
        double a = info[0]->IsUndefined() ? 0 : info[0]->NumberValue(context).FromJust();
        double b = info[1]->IsUndefined() ? 0 : info[1]->NumberValue(context).FromJust();
        double c = info[2]->IsUndefined() ? 0 : info[2]->NumberValue(context).FromJust();

        WrappedPoly* obj = new WrappedPoly(a, b, c);
        obj->Wrap(info.This());
        info.GetReturnValue().Set(info.This());
    }
}