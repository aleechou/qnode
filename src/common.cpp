#include "common.h"
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

using namespace v8;

v8::Local<v8::String> JsonStringify(v8::Isolate * isolate, v8::Local<v8::Value> value){

	v8::Local<v8::Object> global = isolate->GetCurrentContext()->Global();
	v8::Handle<v8::Object> JSON = global->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
	v8::Handle<v8::Function> JSON_stringify = v8::Handle<v8::Function>::Cast(JSON->Get(v8::String::NewFromUtf8(isolate, "stringify")));

	v8::Handle<v8::Value> args[1];
	args[0] = value;

	return JSON_stringify->Call(JSON, 1, args)->ToString();
}

v8::Local<v8::Value> JsonParse(v8::Isolate * isolate, const QByteArray & data){

	v8::Local<v8::Object> global = isolate->GetCurrentContext()->Global();
	v8::Handle<v8::Object> JSON = global->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject();
	v8::Handle<v8::Function> JSON_parse = v8::Handle<v8::Function>::Cast(JSON->Get(v8::String::NewFromUtf8(isolate, "parse")));

	v8::Handle<v8::Value> args[1];
	args[0] = v8::String::NewFromUtf8(isolate, data);

	return JSON_parse->Call(JSON, 1, args);
}


v8::Isolate * globalMainIsolate = nullptr ;

void setMainIsolate(v8::Isolate * isolate) {
    globalMainIsolate = isolate ;
}

v8::Isolate * mainIsolate() {
    return globalMainIsolate ;
}

void runNodeScript(const QString & script) {

    v8::Isolate * isolate = mainIsolate() ;

    if( !isolate ){
        qDebug() << "can not call runScript(), isolate is null" ;
        return ;
    }

    v8::HandleScope scope(isolate);

    v8::Script::Compile ( v8string(script) )->Run();
}


Local<Value> qtjsonToV8(Isolate * isolate, const QJsonValue & value) {
    if(value.isString()) {
        return v8string(value.toString()) ;
    }
    else if(value.isDouble()) {
        return v8int32(value.toDouble()) ;
    }
    else if(value.isBool()) {
        return v8::Boolean::New(isolate, value.toBool()) ;
    }
    else if(value.isArray()) {
        QJsonArray arrValue = value.toArray() ;
        Local<Array> array = Array::New(isolate, arrValue.count()) ;
        for(int i=0; i<arrValue.count(); i++) {
            array->Set(i, qtjsonToV8(isolate, arrValue.at(i))) ;
        }
        return array ;
    }
    else if(value.isObject()) {
        QJsonObject objValue = value.toObject() ;
        Local<Object> object = Object::New(isolate) ;
        foreach(QString key, objValue.keys()){
            object->Set(v8string(key), qtjsonToV8(isolate, objValue.value(key))) ;
        }
        return object ;
    }
    else if(value.isNull()) {
        return v8::Null(isolate) ;
    }
    else if(value.isUndefined()) {
        return v8::Undefined(isolate) ;
    }
    else {
        return v8str("--unknow qt type--") ;
    }
}

Local<Value> qvariantToV8(Isolate * isolate, const QVariant & value) {
    int type = value.type() ;
    if( type == QVariant::String ){
        return v8string(value.toString()) ;
    }
    else if( type == QVariant::Int || type == QVariant::UInt || type == QVariant::LongLong || type == QVariant::ULongLong ){
        return v8int32(value.toULongLong()) ;
    }
    else if( type == QVariant::Double ){
        return v8::Number::New(isolate, value.toDouble()) ;
    }
    else if( type == QVariant::Bool ){
        return v8::Boolean::New(isolate, value.toBool()) ;
    }

    // json
    else if( type == QMetaType::QJsonObject || type == QMetaType::QJsonArray || type == QMetaType::QJsonValue ){
        return qtjsonToV8(isolate, value.toJsonValue()) ;
    }
    else {
        return v8str("--unknow qt type--") ;
    }
}
