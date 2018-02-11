const EventEmitter = require('events');
const qnode = require(process.env.QNODEPATH || "../../.bin/qnode.node")

function stringifyArguments(passedArgvs) {
    var vars = [],
        values = []
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
    constructor(className) {
        super(className || "BrowserWindow*")

        Window.windowsPoolById[this.id()] = this
    }

    id() {
        return this.nativeWindowId()
    }

    aload(url) {
        this.load(url)
        var self = this
        return new Promise((resolve, reject) => {
            this.once('ready', function(doneUrl, ok) {
                ok ? resolve(doneUrl) : reject()
            })
        })
    }

    run(func, passedArgvs) {
        var [vars, values] = stringifyArguments(passedArgvs)

        return this.runScript(`
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
        var invoke_id = Window.run_invoke_id++;
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

    aloadScript(url) {
        var invokeId = Window.run_invoke_id++;
        this.loadScript(url, invokeId)

        return new Promise((resolve) => {
            this.on("scriptLoaded", onemit)

            var self = this

            function onemit(url, id) {
                if (id == invokeId) {
                    resolve()
                    self.removeListener('scriptLoaded', onemit)
                }
            }
        })
    }

    async bridgeObject(object, funcReceiverInWindow) {
        if (!funcReceiverInWindow)
            funcReceiverInWindow = () => {}

        object["@"].bridgeTo.push(this)

        await this.arun(() => {

            shadowObject = qnode.bridge.object(object)

            funcReceiverInWindow(shadowObject)
            resolve()

        }, { object, funcReceiverInWindow })
    }
}

Window.run_invoke_id = 0
Window.run_callbacks = {}
Window.windowsPoolById = {}

Window.byId = function(id) {
    return Window.windowsPoolById[id]
}

global.qnode_Window_on_run_resolve = function(invokeId, val) {
    if (!Window.run_callbacks[invokeId]) {
        return
    }
    Window.run_callbacks[invokeId](val)
    delete Window.run_callbacks[invokeId]
    setImmediate(() => {})
}




qnode.Window = Window