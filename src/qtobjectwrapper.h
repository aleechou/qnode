#ifndef QTOBJECTWRAPPER_H
#define QTOBJECTWRAPPER_H

#include <QObject>
#include <node.h>
#include <node_object_wrap.h>

class QtObjectWrapper : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> exports);

 private:
  explicit QtObjectWrapper(int className);
  ~QtObjectWrapper();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  static void invoke(const v8::FunctionCallbackInfo<v8::Value>& args);

  const QMetaObject * metaObject = nullptr ;
  QObject * object = nullptr ;


};


#endif // QTOBJECTWRAPPER_H
