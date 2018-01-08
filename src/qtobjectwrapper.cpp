#include "qtobjectwrapper.h"
#include "common.h"
#include <QMetaMethod>

using namespace v8;

Persistent<Function> QtObjectWrapper::constructor;

QtObjectWrapper::QtObjectWrapper(int typeId)
{
    metaObject = QMetaType(typeId).metaObject();

    qdd
    this->object = metaObject->newInstance();
    qdd

}

QtObjectWrapper::~QtObjectWrapper() {
    if(object!=nullptr) {
        object->deleteLater();
    }
}

void QtObjectWrapper::Init(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "QtObjectWrapper"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "invoke", invoke);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "QtObjectWrapper"),
               tpl->GetFunction());
}

void QtObjectWrapper::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
qdd
    // Invoked as constructor: `new QtObjectWrapper(...)`
    if (args.IsConstructCall()) {
        QByteArray className = qtstring(args[0]) ;
qdd
        int typeId = QMetaType::type(className);
        if(QMetaType::UnknownType==typeId) {
            Throw("unknow qt class, you must call qRegisterMetaType first.")
        }
        qdd
        QtObjectWrapper* obj = new QtObjectWrapper(typeId);
        qdd
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    }

    // Invoked as plain function `QtObjectWrapper(...)`, turn into construct call.
    else {
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        args.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
}

void QtObjectWrapper::invoke(const FunctionCallbackInfo<Value>& args) {

    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    QtObjectWrapper* wrapper = ObjectWrap::Unwrap<QtObjectWrapper>(args.Holder());

    if(wrapper->object==nullptr || wrapper->metaObject==nullptr) {
        Throw("wrapper->object==nullptr || wrapper->metaObject==nullptr")
    }

    QString methodName = qtstring(args[0]) ;


    int index = wrapper->metaObject->indexOfMethod(methodName.toStdString().c_str());
    if(index<0) {
        qDebug() << "unknow method" << methodName << "for class" << wrapper->metaObject->className() ;
        Throw("unknow method")
        return ;
    }

    qd(index)

    QMetaMethod metaMethod = wrapper->metaObject->method(index);

    qdd

    qDebug() << wrapper->object << metaMethod.isValid() ;

    metaMethod.invoke(wrapper->object) ;

}
