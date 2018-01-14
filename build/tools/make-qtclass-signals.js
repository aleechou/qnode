const fs = require("fs")
const qnode = require("../../")

var qtClasses = [
    'QObject*', 
    'BrowserWindow*',
]


var qtNumberTypes = ['int', 'unsigned int']


function parseClassInfo(typeName) {
    var typeid = eval(qnode.qtTypeId(typeName))
    var methods = eval(qnode.qtClassMeta(typeName))

    var className = typeName.replace(/\*/g,'').trim()

    // console.log("typeid",typeid)


    var cppcode = `

    /* start of class ${className} */
    case ${typeid}:
        switch(sigindex){
`

    methods.forEach((meta)=>{
        if(meta.type!=1) return

        // console.log(meta.params)
        var cppParamList = []
        var paramConv = []
        meta.params.forEach((paramMeta, i)=>{

            var paramName = 'arg'+(i+1)
            var paramTitle = paramMeta.type.match(/\*/)? paramMeta.type: `const ${paramMeta.type} & ${paramName}`

            cppParamList.push(paramTitle)

            if(paramMeta.type=='QString') {
                paramConv.push(`                argv[${i+1}] = Local<Value>::New(isolate, v8string(${paramName})) ;`)
            }
            else if(paramMeta.type=='bool') {
                paramConv.push(`                argv[${i+1}] = Local<Value>::New(isolate, v8::Boolean::New(isolate, ${paramName})) ;`)
            }
            else if(qtNumberTypes.includes(paramMeta.type)) {
                paramConv.push(`                argv[${i+1}] = Local<Value>::New(isolate, v8int32(${paramName})) ;`)
            }
            else {
                paramConv.push(`                argv[${i+1}] = Local<Value>::New(isolate, v8str("unknow qt type (${paramMeta.type})")) ;`)
            }
        })
        console.log(meta.signature)
        // console.log(meta.idx, className, meta.signature)
        // Local<Value>::New(isolate, v8string(signalSignature))

        var funccode = `[wrapper,isolate](${cppParamList.join(', ')}){
                GET_NODEJS_LISTNER ;

                const unsigned argc = ${meta.params.length+1};
                Local<Value> argv[argc] = { Local<Value>::New(isolate, v8str("${meta.signature}")) };
${paramConv.join("\\\r\n")}
                method->Call(wrapper->handle(), argc, argv);
            }`

        cppcode+= `
        case ${meta.idx}:
            QObject::connect((${className}*)wrapper->object, &${className}::${meta.name}, postman, ${funccode}) ;
            break;
`
    })

    cppcode+= `
        default:
            Throw("unknow sigindex")
            return ;
        }

        /* end of class ${className} */
        break;
`

    return cppcode
}

function parseAllClasses() {

    var cppcode = `
switch (wrapper->typeId) {
`

    qtClasses.forEach((className)=>{
        cppcode+= parseClassInfo(className)
    })
    

    cppcode+= `
default:
    Throw("unknow typeId")
    return;
}
`

    return cppcode
}


function makeCpp() {
    var cppcode = parseAllClasses(qtClasses)

    return `
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
` + cppcode.split('\n').reduce((arr, line)=>{
        arr.push(line + "    \\")
        return arr
    }, []).join("\r\n") + "\r\n"
}


var cppcode = makeCpp()
fs.writeFile(__dirname+'/../../src/qtsignalrouter.cc',cppcode,(error)=>{
    if(error) console.error(error)
    process.exit()
})