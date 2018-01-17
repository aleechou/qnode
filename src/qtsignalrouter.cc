
#define GET_NODEJS_LISTNER \
    \
    HandleScope scope(isolate);\
    \
    Local<Value> member = wrapper->persistent().Get(isolate)->Get(v8str("onQtSignalReceived")) ; \
    if( !member->IsFunction() ) { \
        qDebug() << "onQtSignalReceived is " << qtstring(member->TypeOf(isolate)) ; \
        return ; \
    } \
    \
    Local<Function> method = Local<Function>::Cast(member); 

    

#define ConnectSignalAndSlot \    
    \
switch (wrapper->typeId) {    \
    \
    \
    /* start of class QObject */    \
    case 39:    \
        switch(sigindex){    \
    \
        case 0:    \
            QObject::connect((QObject*)wrapper->object, &QObject::destroyed, postman, [wrapper,isolate](QObject*){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 2;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("destroyed(QObject*)")) };    \
                argv[1] = Local<Value>::New(isolate, v8str("unknow qt type (QObject*)")) ;    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        case 1:    \
            QObject::connect((QObject*)wrapper->object, &QObject::destroyed, postman, [wrapper,isolate](){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 1;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("destroyed()")) };    \
    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        case 2:    \
            QObject::connect((QObject*)wrapper->object, &QObject::objectNameChanged, postman, [wrapper,isolate](const QString & arg1){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 2;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("objectNameChanged(QString)")) };    \
                argv[1] = Local<Value>::New(isolate, v8string(arg1)) ;    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        default:    \
            Throw("unknow sigindex")    \
            return ;    \
        }    \
    \
        /* end of class QObject */    \
        break;    \
    \
    \
    /* start of class BrowserWindow */    \
    case 1029:    \
        switch(sigindex){    \
    \
        case 0:    \
            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::destroyed, postman, [wrapper,isolate](QObject*){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 2;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("destroyed(QObject*)")) };    \
                argv[1] = Local<Value>::New(isolate, v8str("unknow qt type (QObject*)")) ;    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        case 1:    \
            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::destroyed, postman, [wrapper,isolate](){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 1;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("destroyed()")) };    \
    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        case 2:    \
            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::objectNameChanged, postman, [wrapper,isolate](const QString & arg1){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 2;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("objectNameChanged(QString)")) };    \
                argv[1] = Local<Value>::New(isolate, v8string(arg1)) ;    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        case 5:    \
            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::windowTitleChanged, postman, [wrapper,isolate](const QString & arg1){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 2;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("windowTitleChanged(QString)")) };    \
                argv[1] = Local<Value>::New(isolate, v8string(arg1)) ;    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        case 6:    \
            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::windowIconChanged, postman, [wrapper,isolate](const QIcon & arg1){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 2;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("windowIconChanged(QIcon)")) };    \
                argv[1] = Local<Value>::New(isolate, v8str("unknow qt type (QIcon)")) ;    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        case 7:    \
            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::windowIconTextChanged, postman, [wrapper,isolate](const QString & arg1){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 2;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("windowIconTextChanged(QString)")) };    \
                argv[1] = Local<Value>::New(isolate, v8string(arg1)) ;    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        case 8:    \
            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::customContextMenuRequested, postman, [wrapper,isolate](const QPoint & arg1){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 2;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("customContextMenuRequested(QPoint)")) };    \
                argv[1] = Local<Value>::New(isolate, v8str("unknow qt type (QPoint)")) ;    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        case 32:    \
            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::ready, postman, [wrapper,isolate](const QString & arg1, const bool & arg2){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 3;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("ready(QString,bool)")) };    \
                argv[1] = Local<Value>::New(isolate, v8string(arg1)) ;\    \
                argv[2] = Local<Value>::New(isolate, v8::Boolean::New(isolate, arg2)) ;    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        case 33:    \
            QObject::connect((BrowserWindow*)wrapper->object, &BrowserWindow::scriptLoaded, postman, [wrapper,isolate](const QString & arg1, const uint & arg2){    \
                GET_NODEJS_LISTNER ;    \
    \
                const unsigned argc = 3;    \
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("scriptLoaded(QString,uint)")) };    \
                argv[1] = Local<Value>::New(isolate, v8string(arg1)) ;\    \
                argv[2] = Local<Value>::New(isolate, v8int32(arg2)) ;    \
                method->Call(wrapper->handle(), argc, argv);    \
            }) ;    \
            break;    \
    \
        default:    \
            Throw("unknow sigindex")    \
            return ;    \
        }    \
    \
        /* end of class BrowserWindow */    \
        break;    \
    \
default:    \
    Throw("unknow typeId")    \
    return;    \
}    \
    \
