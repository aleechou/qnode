#include "qtobjectwrapper.h"
#include "common.h"
#include <QMetaMethod>
#include "dynamicconnectionreceiver.h"
#include "qtsignalrouter.cc"

using namespace v8;

Persistent<Function> QtObjectWrapper::constructor;

QtObjectWrapper::QtObjectWrapper(int typeId, Isolate* isolate)
    : isolate(isolate)
    , typeId(typeId)
    , ObjectWrap()
{
    metaObject = QMetaType(typeId).metaObject();
    this->object = metaObject->newInstance();
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
    NODE_SET_PROTOTYPE_METHOD(tpl, "methodList", methodList);
    NODE_SET_PROTOTYPE_METHOD(tpl, "connectQtSignal", connectQtSignal);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "QtObjectWrapper"),
               tpl->GetFunction());
}

void QtObjectWrapper::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    // Invoked as constructor: `new QtObjectWrapper(...)`
    if (args.IsConstructCall()) {
        QByteArray className = qtstring(args[0]) ;
        int typeId = QMetaType::type(className);
        if(QMetaType::UnknownType==typeId) {
            Throw("unknow qt class, you must call qRegisterMetaType first.")
        }

        QtObjectWrapper* obj = new QtObjectWrapper(typeId, isolate);

        obj->Wrap(args.This());
        args.This()->Set(v8str("className"), args[0]) ;
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

#define toQtArgs \
    QVariantList invokeArgs ;                                               \
    for(int i=1; i<args.Length(); i++){                                     \
        if( args[i]->IsInt32() ) {                                          \
            invokeArgs.append(QVariant(args[i]->ToInt32()->Value())) ;      \
        }                                                                   \
        else if( args[i]->IsBoolean() ){                                    \
            invokeArgs.append(QVariant(args[i]->ToBoolean()->Value())) ;    \
        }                                                                   \
        else if( args[i]->IsString() ){                                     \
            invokeArgs.append(QVariant( qtstring(args[i]) )) ;             \
        }                                                                   \
        else {                                                              \
            qDebug() << "unsuported args type: " << ToQString(args[i]) ;    \
            return ;                                                        \
        }                                                                   \
    }


#define invokeargument(idx) \
            argsNum>idx? arguments.value(idx): QGenericArgument()

void QtObjectWrapper::invoke(const FunctionCallbackInfo<Value>& args) {

    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    QtObjectWrapper* wrapper = ObjectWrap::Unwrap<QtObjectWrapper>(args.Holder());

    if(wrapper->object==nullptr || wrapper->metaObject==nullptr) {
        Throw("wrapper->object==nullptr || wrapper->metaObject==nullptr")
        return ;
    }

    QString methodName = qtstring(args[0]) ;

    int index = wrapper->metaObject->indexOfMethod(methodName.toStdString().c_str());
    if(index<0) {
        qDebug() << "unknow method" << methodName << "for class" << wrapper->metaObject->className() ;
        Throw("unknow method")
        return ;
    }

    QMetaMethod metaMethod = wrapper->metaObject->method(index);

    QList<QGenericArgument> arguments;
    QVariantList invokeArgs ;
    for(int i=1; i<args.Length(); i++){
        if( args[i]->IsInt32() ) {
            invokeArgs.append(QVariant(args[i]->ToInt32()->Value())) ;
        }
        else if( args[i]->IsBoolean() ){
            invokeArgs.append(QVariant(args[i]->ToBoolean()->Value())) ;
        }
        else if( args[i]->IsString() ){
            invokeArgs.append(QVariant( qtstring(args[i]) )) ;
        }
        else {
            qDebug() << "unsuported args type: " << qtstring(args[i]) ;
            return ;
        }

        arguments << QGenericArgument (
            QMetaType::typeName(invokeArgs.at(i-1).userType()),
            const_cast<void*>(invokeArgs.at(i-1).constData())
        );
    }


    if( strcmp(metaMethod.typeName(),"void")==0 ) {

        // Perform the call
        int argsNum = args.Length() - 1 ;
        if(! metaMethod.invoke(
            wrapper->object,
            Qt::DirectConnection,
            invokeargument(0),
            invokeargument(1),
            invokeargument(2),
            invokeargument(3),
            invokeargument(4),
            invokeargument(5),
            invokeargument(6),
            invokeargument(7),
            invokeargument(8),
            invokeargument(9)
        ) ) {
            qWarning() << "Calling" << metaMethod.methodSignature() << "failed.";
            return ;
        }
    }
    else {

        q metaMethod.typeName() ;
        q QMetaType::type(metaMethod.typeName()) ;
        QVariant returnValue(QMetaType::type(metaMethod.typeName()),
            static_cast<void*>(NULL));

        QGenericReturnArgument returnArgument(
            metaMethod.typeName(),
            const_cast<void*>(returnValue.constData())
        );

        // Perform the call
        int argsNum = args.Length() - 1 ;
        if(! metaMethod.invoke(
            wrapper->object,
            Qt::DirectConnection,
            returnArgument,
            invokeargument(0),
            invokeargument(1),
            invokeargument(2),
            invokeargument(3),
            invokeargument(4),
            invokeargument(5),
            invokeargument(6),
            invokeargument(7),
            invokeargument(8),
            invokeargument(9)
        ) ) {
            qWarning() << "Calling" << metaMethod.methodSignature() << "failed.";
            return ;
        }
    }
}

void QtObjectWrapper::methodList(const FunctionCallbackInfo<Value>& args) {

    QtObjectWrapper* wrapper = ObjectWrap::Unwrap<QtObjectWrapper>(args.Holder());
    const QMetaObject* metaObject = wrapper->object->metaObject();

    Isolate* isolate = args.GetIsolate();

    args.GetReturnValue().Set(v8string(methodList(metaObject)));
}


QString QtObjectWrapper::methodList(const QMetaObject * metaObject) {

    QString output = "[\r\n" ;

    for(int i = 0; i < metaObject->methodCount(); ++i) {
        QMetaMethod metaMethod = metaObject->method(i) ;
        output+= "  {\r\n" ;
        output+= "    name:\"" + QString(metaMethod.name()) + "\",\r\n" ;
        output+= "    type:\"" + QString("%1").arg(metaMethod.methodType()) + "\",\r\n" ;
        output+= "    returnType:\"" + QString(metaMethod.typeName()) + "\",\r\n" ;
        output+= "    idx:\"" + QString("%1").arg(metaMethod.methodIndex()) + "\",\r\n" ;

        output+= "    params:[\r\n" ;
        QList<QByteArray> parameterNames = metaMethod.parameterNames() ;
        QList<QByteArray> parameterTypes = metaMethod.parameterTypes() ;
        for(int p=0;p<metaMethod.parameterCount();p++){
            output+= "      {\r\n" ;
            output+= "        name: \"" +QString(parameterNames[p])+ "\",\r\n" ;
            output+= "        type: \"" +QString(parameterTypes[p])+ "\",\r\n" ;
            output+= "      },\r\n" ;

        }
        output+= "    ],\r\n" ;

        output+= "    signature:\"" + QString(metaMethod.methodSignature()) + "\",\r\n" ;
        output+= "  },\r\n" ;
    }

    output+= "]" ;

    return output ;
}



#include "browserwindow.h"
#include <typeinfo>
//typedef void (BrowserWindow::*ready)(QString, bool);

void QtObjectWrapper::connectQtSignal(const FunctionCallbackInfo<Value>& args) {

    v8::Isolate* isolate = args.GetIsolate();
    QtObjectWrapper* wrapper = ObjectWrap::Unwrap<QtObjectWrapper>(args.Holder());

    int sigindex = args[0]->ToInt32()->Value() ;
//    qDebug() << signalSignature.toStdString().c_str() ;

//    if( wrapper->signalReceivers.keys().contains(signalSignature) ){
//        return ;
//    }

//    const QMetaObject * metaObj = wrapper->object->metaObject() ;
//    int sigindex = metaObj->indexOfSignal(signalSignature.toStdString().c_str()) ;
//    if(sigindex<0) {
//        Throw(QString("unknow signal %1 of class %2").arg(signalSignature).arg(metaObj->className()).toStdString().c_str())
//        return ;
//    }

    DynamicConnectionReceiver * receiver = new DynamicConnectionReceiver(wrapper) ;
    const QMetaObject * recvMeta = receiver->metaObject() ;

    ConnectSignalAndSlot


//    wrapper->signalReceivers.insert(signalSignature, receiver) ;
}

void QtObjectWrapper::onSignalReceived(QObject * receiver) {

    HandleScope scope(isolate);

    QString signalSignature = signalReceivers.key(receiver) ;

    Local<Value> member = persistent().Get(isolate)->Get(v8str("onQtSignalReceived")) ;
    if( !member->IsFunction() ) {
        qDebug() << "onQtSignalReceived is " << qtstring(member->TypeOf(isolate)) ;
        return ;
    }

    Local<Function> method = Local<Function>::Cast(member);

    const unsigned argc = 1;
    Local<Value> argv[argc] = { Local<Value>::New(isolate, v8string(signalSignature)) };
    method->Call(handle(), argc, argv);
}



