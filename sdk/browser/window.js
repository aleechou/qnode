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

$window.runInNode = function(func, passedArgvs) {

    var [vars, values] = stringifyArguments(passedArgvs)

    return $window.runScriptInMainIsolate(`
        (function(${vars.join(',')}){
        try{
            (${func.toString()})()
        }catch(e){
            console.error(e)
        }
        })( ${values.join(',')} )
    `)
}

$window.arunInNode = function(func, passedArgvs) {
    var invoke_id = run_invoke_id++;
    var [vars, values] = stringifyArguments(passedArgvs)

    var strfunc = `
var resolve = function(val){
    $$qnode$$.Window.byId(${$window.id()}).run("qnode_Window_on_run_resolve(${invoke_id}, " + JSON.stringify(val) + ")")
}
;(function(${vars.join(',')}){
    try{
        (${func.toString()})()
    }catch(e){
        console.error(e)
    }
})( ${values.join(',')} )`

    $window.runScriptInMainIsolate(strfunc)

    return new Promise((resolve) => {
        run_callbacks[invoke_id] = resolve
    })
}



var run_invoke_id = 0
var run_callbacks = {}

window.qnode_Window_on_run_resolve = function(invokeId, val) {
    if (!run_callbacks[invokeId]) {
        return
    }
    run_callbacks[invokeId](val)
    delete run_callbacks[invokeId]
    setImmediate(() => {})
}


$window.bridgeEvent = new EventEmitter()

for (var methodName in ['on', 'once']) {;
    ((methodName) => {
        $window[methodName] = function() {
            return $window.bridgeEvent[methodName].apply($window.bridgeEvent, arguments)
        }
    })(methodName)
}


$window.emit = function(eventName, ...argv) {
    // 触发本地监听
    $window.bridgeEvent.emit(eventName, ...argv)

    // 向 Nodejs 触发事件
    $window.runInNode(() => {
        $$qnode$$.Window.byId(windowId).emit(eventName, ...argv)
    }, { windowId: $window.id, eventName, argv })
}