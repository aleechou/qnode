#!/usr/bin/env node

const fs = require("fs")

var qtBaseClasses = [
    'QObject*',
    'BrowserWindow*',
    'QxtGlobalShortcut*',
]


var qtNumberTypes = ['int', 'unsigned int', 'uint', 'long', 'ulong', 'longlong', 'ulonglong']


function parseClassInfo(qnode, typeName) {

    if (!qnode) {
        qnode = require("../")
    }

    var typeid = eval(qnode.qtTypeId(typeName))
    var methods = eval(qnode.qtClassMeta(typeName))

    var className = typeName.replace(/\*/g, '').trim()


    var cppcode = `

    /* start of class ${className} */
    case ${typeid}:
        switch(sigindex){
`

    methods.forEach((meta) => {
        if (meta.type != 1) return

        var cppParamList = []
        var paramConv = []
        meta.params.forEach((paramMeta, i) => {

            var isCppPointer = paramMeta.type.match(/\*/)
            var paramName = 'arg' + (i + 1)
            var paramTitle = isCppPointer ? paramMeta.type : `const ${paramMeta.type} & ${paramName}`

            cppParamList.push(paramTitle)

            if (paramMeta.type == 'QString') {
                paramConv.push(`                argv[${i+1}] = Local<Value>::New(isolate, v8string(${paramName})) ;`)
            } else if (paramMeta.type == 'bool') {
                paramConv.push(`                argv[${i+1}] = Local<Value>::New(isolate, v8::Boolean::New(isolate, ${paramName})) ;`)
            } else if (qtNumberTypes.includes(paramMeta.type)) {
                paramConv.push(`                argv[${i+1}] = Local<Value>::New(isolate, v8int32(${paramName})) ;`)
            } else if (['QJsonObject', 'QJsonArray', 'QJsonValue'].includes(paramMeta.type)) {
                paramConv.push(`                argv[${i+1}] = qtjsonToV8(isolate, ${paramName}) ;`)
            } else {
                paramConv.push(`                argv[${i+1}] = Local<Value>::New(isolate, v8str("unknow qt type (${paramMeta.type})")) ;`)
            }
        })
        console.log(className + "(" + typeid + ")", meta.idx, meta.signature)

        var funccode = `[wrapper,isolate](${cppParamList.join(', ')}){
                GET_NODEJS_LISTNER ;

                const unsigned argc = ${meta.params.length+1};
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("${meta.signature}")) };
${paramConv.join("\\\r\n")}
                method->Call(wrapper->handle(), argc, argv);
            }`

        cppcode += `
        case ${meta.idx}:
            QObject::connect((${className}*)wrapper->object, &${className}::${meta.name}, postman, ${funccode}) ;
            break;
`
    })

    cppcode += `
        default:
            Throw( QString("unknow sigindex: %1, for qt native class: QxtGlobalShortcut (typeId: %2)").arg(sigindex).arg(wrapper->m_typeId).toStdString().c_str() )
            return ;
        }

        /* end of class ${className} */
        break;
`

    return cppcode
}

function parseAllClasses(qnode, qtClasses) {

    var cppcode = `
switch (wrapper->m_typeId) {
`

    qtClasses.forEach((className) => {
        cppcode += parseClassInfo(qnode, className)
    })


    cppcode += `
    default:
        Throw( QString("unknow typeId: %1").arg(wrapper->m_typeId).toStdString().c_str() )
        return;
}
`

    return cppcode
}


function makeCpp(qnode, qtClasses) {

    var cppIncludes = ""
    Object.values(qtClasses).forEach((path) => {
        cppIncludes += `#include "${path}"\r\n`
    })

    var cppcode = parseAllClasses(qnode, qtBaseClasses.concat(Object.keys(qtClasses)))

    return `
${cppIncludes}


#define GET_NODEJS_LISTNER \\
    \\
    HandleScope scope(isolate);\\
    \\
    Local<Value> member = wrapper->persistent().Get(isolate)->Get(v8str("onQtSignalReceived")) ; \\
    if( !member->IsFunction() ) { \\
        qDebug() << "onQtSignalReceived is " << qtstring(member->TypeOf(isolate)) ; \\
        return ; \\
    } \\
    \\
    Local<Function> method = Local<Function>::Cast(member); 

    

#define ConnectSignalAndSlot \\
` + cppcode.split('\n').reduce((arr, line) => {
        arr.push(line + "    \\")
        return arr
    }, []).join("\r\n") + "\r\n\r\n\r\n"
}

module.exports = function(qnode, outputPath, nativeClasses) {
    var cppcode = makeCpp(qnode, nativeClasses || {})

    console.log("write cpp file .. ", outputPath)
    fs.writeFileSync(outputPath, cppcode)
    console.log("done")
}