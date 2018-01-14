
const EventEmitter = require('events');
const qnode = require("../.bin/qnode.node")

function stringifyArguments(passedArgvs) {
    var vars = [], values = []
    if (passedArgvs) {
        for (var argvName in passedArgvs) {
            vars.push(argvName)
            switch (typeof passedArgvs[argvName]) {
                case 'function':
                    values.push(passedArgvs[argvName].toString())
                    break
                case 'undefined':
                    values.push('undefined')
                    break
                default:
                    values.push(JSON.stringify(passedArgvs[argvName]))
                    break
            }
        }
    }
    return [vars, values]
}


class Window extends qnode.QtObjectWrapper {
    constructor() {
        super("BrowserWindow*")
    }

    aload(url) {
        this.load(url)
        var self= this
        return new Promise((resolve, reject)=>{
            this.once('ready',function (doneUrl, ok){
                ok? resolve(doneUrl): reject()
            })
        })
    }

    run(func, passedArgvs) {
        [vars, values] = stringifyArguments(passedArgvs)

        this.runScript(`
            (function(${vars.join(',')}){
            try{
            (${func.toString()})()
            }catch(e){
                console.error(e)
            }
            })( ${values.join(',')} )
        `)
    }

    arun(func, passedArgvs) {
        var invoke_id = Window.run_invoke_id++
        var [vars, values] = stringifyArguments(passedArgvs)
        
        var strfunc = `
    var resolve = function(val){
        $window.runScriptInMainIsolate("qnode_Window_on_run_resolve(${invoke_id}, " + JSON.stringify(val) + ")")
    }
    ;(function(${vars.join(',')}){
    try{
    (${func.toString()})()
    }catch(e){
        console.error(e)
    }
    })( ${values.join(',')} )`
    
        this.runScript(strfunc)
    
        return new Promise((resolve) => {
            Window.run_callbacks[invoke_id] = resolve
        })
    }

}

Window.run_invoke_id = 0
Window.run_callbacks = {}

global.qnode_Window_on_run_resolve = function(invokeId, val) {
    if (Window.run_callbacks[invokeId]) {
        Window.run_callbacks[invokeId]( val )
        delete Window.run_callbacks[invokeId]
    }
}

qnode.Window = Window
// qnode.window.create = async function() {
//     return new Promise((resolve)=>{
//        var window = new Window()
//        qnode.api.invoke($qnodeapi_browser_window_creator, "createBrowserWindow(uint)", $qnodeapi_thread)
//            .then((objId)=>{
//                 window._oncreated(objId)
//                 resolve(window)
//              })
//        return window
//     })
// }