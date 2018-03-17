#include "qtobjectwrapper.h"
#include "common.h"
#include <QMetaMethod>
#include <QJsonObject>
#include "qxtglobalshortcut5/qxtglobalshortcut.h"
#include "QHotkey/qhotkey.h"
#include "browserwindow.h"
#include "wrapper/MediaPlayer.h"
#include "wrapper/SerialPort.h"

#ifdef QtSignalRouter
#include "qtsignalrouter.cc"
#endif

using namespace v8;

Persistent<Function> QtObjectWrapper::constructor;

QtObjectWrapper::QtObjectWrapper(int typeId, Isolate* isolate)
    : ObjectWrap()
    , metaObject(nullptr)
    , object(nullptr)
    , isolate(isolate)
    , m_typeId(typeId)
{
    metaObject = QMetaType(m_typeId).metaObject();
    this->object = metaObject->newInstance();
}

QtObjectWrapper::~QtObjectWrapper() {
    if(object!=nullptr) {
        object->deleteLater();
    }
}

QObject * QtObjectWrapper::qtObject(){
    return object ;
}
int QtObjectWrapper::qtTypeId(){
    return m_typeId ;
}

void QtObjectWrapper::Init(Handle<Object> exports) {
    Isolate* isolate = Isolate::GetCurrent();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "QtObjectWrapper"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "typeId", typeId);
    NODE_SET_PROTOTYPE_METHOD(tpl, "invoke", invoke);
    NODE_SET_PROTOTYPE_METHOD(tpl, "methodList", methodList);
    NODE_SET_PROTOTYPE_METHOD(tpl, "connectQtSignal", connectQtSignal);
    NODE_SET_PROTOTYPE_METHOD(tpl, "disconnectQtSignal", disconnectQtSignal);

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
            Throw(QString("unknow qt class: %1, you must call qRegisterMetaType first.").arg(QString(className)).toStdString().c_str()) ;
            return ;
        }

        QtObjectWrapper* obj = new QtObjectWrapper(typeId, isolate);

        if(!obj->object) {
            Throw(QString("cannot new a native class %1, maybe the constructor not declare with Q_INVOKABLE.").arg(QString(className)).toStdString().c_str()) ;
            return ;
        }


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

#include <QVariantList>

QJsonArray QtObjectWrapper::v8arrayToQt(Isolate * isolate, const Local<Array> & value) {
    QJsonArray array ;
    for(uint i=0; i<value->Length(); i++){
        array.append(v8ToQJson(isolate, value->Get(i))) ;
    }
    return array ;
}


QJsonValue QtObjectWrapper::v8ToQJson(Isolate * isolate, const Local<Value> & value){

    if( value->IsInt32() ) {
        return QJsonValue(value->ToInt32()->Value()) ;
    }
    else if( value->IsNumber() ) {
        return QJsonValue(value->ToNumber()->Value()) ;
    }
    else if( value->IsBoolean() ){
        return QJsonValue(value->ToBoolean()->Value()) ;
    }
    else if( value->IsString() ){
        return QJsonValue( qtstring(value) ) ;
    }
    else if( value->IsArray() ){
        Local<Array> arr = Local<Array>::Cast(value);
        return v8arrayToQt(isolate,arr) ;
    }
    else {
        return QJsonValue() ;
    }
}

QVariant QtObjectWrapper::v8ToQt(Isolate * isolate, const Local<Value> & value){

    if( value->IsInt32() ) {
        return QVariant(value->ToInt32()->Value()) ;
    }
    else if( value->IsNumber() ) {
        return QVariant(value->ToNumber()->Value()) ;
    }
    else if( value->IsBoolean() ){
        return QVariant(value->ToBoolean()->Value()) ;
    }
    else if( value->IsString() ){
        return QVariant( qtstring(value) ) ;
    }
    else if( value->IsArrayBuffer() ){
        Local<ArrayBuffer> buff = Local<ArrayBuffer>::Cast(value) ;
        QByteArray data((char *)buff->GetContents().Data(), buff->GetContents().ByteLength()) ;
        return QVariant(data) ;
    }
    else if( value->IsArray() ){
        Local<Array> arr = Local<Array>::Cast(value);
        return v8arrayToQt(isolate,arr) ;
//            QByteArray data((char *)arr->GetContents().Data()
    }
    else if(value->IsObject() && value->ToObject()->Has(v8str("typeId"))) {
        QtObjectWrapper * argv = ObjectWrap::Unwrap<QtObjectWrapper>(value->ToObject());
        return QVariant::fromValue((void *)argv->object) ;
    }
    else {
        return QVariant() ;
    }
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

    int index = args[0]->ToInt32()->Value();
    if(index<0) {
        qDebug() << "unknow method index" << index << "for class" << wrapper->metaObject->className() ;
        Throw("unknow method")
        return ;
    }

    QMetaMethod metaMethod = wrapper->metaObject->method(index);

    QList<QGenericArgument> arguments;
    QVariantList invokeArgs ;
    for(int i=1; i<args.Length(); i++){

        QVariant var = v8ToQt(isolate, args[i]) ;

        if(var.type()==QVariant::Invalid) {
            qDebug() << "unsuported arg type, value: " << qtstring(args[i])
                     << QString(", arg%1 %2(%3) of %4::%5()")
                            .arg(i)
                            .arg(metaMethod.parameterNames().at(i).toStdString().c_str())
                            .arg(QMetaType::typeName(metaMethod.parameterType(i)))
                            .arg(wrapper->metaObject->className())
                            .arg(metaMethod.name().toStdString().c_str()) ;
            return ;
        }

        invokeArgs.append(var) ;

        arguments << QGenericArgument (
            QMetaType::typeName(invokeArgs.at(i-1).userType()),
            const_cast<void*>(invokeArgs.at(i-1).constData())
        );
    }

    //
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

        args.GetReturnValue().Set(qvariantToV8(isolate, returnValue)) ;
    }
}

void QtObjectWrapper::methodList(const FunctionCallbackInfo<Value>& args) {

    QtObjectWrapper* wrapper = ObjectWrap::Unwrap<QtObjectWrapper>(args.Holder());
    if(wrapper==nullptr) {
        Throw("object wrapper is invalid.") ;
        return ;
    }
    if(wrapper->object==nullptr) {
        Throw("not create native object.") ;
        return ;
    }

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


void QtObjectWrapper::connectQtSignal(const FunctionCallbackInfo<Value>& args) {

    v8::Isolate* isolate = args.GetIsolate();
    QtObjectWrapper* wrapper = ObjectWrap::Unwrap<QtObjectWrapper>(args.Holder());
    int sigindex = args[0]->ToInt32()->Value() ;

    if( wrapper->signalReceivers.keys().contains(sigindex) ) {
        args.GetReturnValue().Set(v8::Boolean::New(isolate, false)) ;
        return ;
    }

    QObject * postman = new QObject(wrapper->object) ;

#ifdef ConnectSignalAndSlot
    ConnectSignalAndSlot ;
#endif

#ifndef ConnectSignalAndSlot
    qDebug() << "no macro: ConnectSignalAndSlot" ;
#endif

    wrapper->signalReceivers.insert(sigindex, postman) ;

    args.GetReturnValue().Set(v8::Boolean::New(isolate, true)) ;
}


void QtObjectWrapper::disconnectQtSignal(const FunctionCallbackInfo<Value>& args) {

    v8::Isolate* isolate = args.GetIsolate();
    QtObjectWrapper* wrapper = ObjectWrap::Unwrap<QtObjectWrapper>(args.Holder());
    int sigindex = args[0]->ToInt32()->Value() ;

    if( !wrapper->signalReceivers.keys().contains(sigindex) ) {
        args.GetReturnValue().Set(v8::Boolean::New(isolate, false)) ;
        return ;
    }

    QObject * postman = wrapper->signalReceivers.value(sigindex, nullptr) ;
    if(postman==nullptr){
        args.GetReturnValue().Set(v8::Boolean::New(isolate, false)) ;
        return ;
    }

    postman->deleteLater();

    args.GetReturnValue().Set(v8::Boolean::New(isolate, true)) ;
}


void QtObjectWrapper::typeId(const FunctionCallbackInfo<Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    QtObjectWrapper* wrapper = ObjectWrap::Unwrap<QtObjectWrapper>(args.Holder());
    args.GetReturnValue().Set(v8int32(wrapper->m_typeId)) ;


}
