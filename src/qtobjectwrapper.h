#ifndef QTOBJECTWRAPPER_H
#define QTOBJECTWRAPPER_H

#include <QVariant>
#include <QJsonValue>
#include <QObject>
#include <QMap>
#include <node.h>
#include <node_object_wrap.h>


class QtObjectWrapper : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> exports);
    static QString methodList(const QMetaObject *);

    QObject * qtObject() ;
    int qtTypeId();

private:
    explicit QtObjectWrapper(int className, v8::Isolate* isolate);
    ~QtObjectWrapper();

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> constructor;

    static void typeId(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void invoke(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void methodList(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void connectQtSignal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void disconnectQtSignal(const v8::FunctionCallbackInfo<v8::Value>& args);


    static QJsonArray v8arrayToQt(v8::Isolate * isolate, const v8::Local<v8::Array> & value) ;
    static QVariant v8ToQt(v8::Isolate * isolate, const v8::Local<v8::Value> & value) ;
    static QJsonValue v8ToQJson(v8::Isolate * isolate, const v8::Local<v8::Value> & value) ;

    const QMetaObject * metaObject = nullptr ;
    QObject * object = nullptr ;
    v8::Isolate* isolate = nullptr ;
    int m_typeId = -1 ;

    QMap<int, QObject*> signalReceivers ;

};


#endif // QTOBJECTWRAPPER_H
