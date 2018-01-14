const fs = require("fs")
const qnode = require("../../")

var qtClasses = [
    'QObject*', 
    
    
    'BrowserWindow*',
]


var validQtTypes = ['QString','bool','int', 'unsigned int']


function parseClassInfo(typeName) {
    var typeid = eval(qnode.qtTypeId(typeName))
    var methods = eval(qnode.qtClassMeta(typeName))

    var className = typeName.replace(/\*/g,'').trim()

    // console.log("typeid",typeid)


    var cppcode = `
    case ${typeid}:
        switch(sigindex){
`

    methods.forEach((meta)=>{
        if(meta.type!=1) return

        // console.log(meta.params)
        var cppParamList = []
        var params = []
        meta.params.forEach((paramMeta, i)=>{

            var paramName = 'arg'+(i+1)
        var paramTitle = paramMeta.type.match(/\*/)? paramMeta.type: `const ${paramMeta.type} & ${paramName}`

            cppParamList.push(paramTitle)

            if(validQtTypes.includes(paramMeta.type)) {
                params.push(paramName)
            }
        })
        console.log(meta.signature, params)
        // console.log(meta.idx, className, meta.signature)

        var funccode = `[](${cppParamList.join(', ')}){
            qDebug() << "${meta.signature} <<<<<<<" ${params.length? (' << '+params.join(' << ')): ''} ;
        }`

        cppcode+= `
        case ${meta.idx}:
            QObject::connect((${className}*)wrapper->object, &${className}::${meta.name}, ${funccode}) ;
            break;
`
    })

    cppcode+= `
    default:
        Throw("unknow sigindex")
        return ;
    }
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
#define ConnectSignalAndSlot \\    
` + cppcode.split('\n').reduce((arr, line)=>{
        arr.push("\t" + line + "    \\")
        return arr
    }, []).join("\r\n") + "\r\n"
}


var cppcode = makeCpp()
fs.writeFile(__dirname+'/../../src/qtsignalrouter.cc',cppcode,(error)=>{
    if(error) console.error(error)
    process.exit()
})