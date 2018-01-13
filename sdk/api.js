const qnode = require("../.bin/qnode.node")
const EventEmitter = require('events');

// qnode = { api: {}, window: {}, classes: {} }

// var invokeReturnCallbacks = {}
// $qnodeapi_invoke_return = function(reqId, value) {
//     if (invokeReturnCallbacks[reqId]) {
//         invokeReturnCallbacks[reqId](value)
//         delete invokeReturnCallbacks[reqId]

//         setTimeout(() => {}, 0);
//     }
// }

// var connectionsSignalSlot = {}
// $qnodeapi_emit = function(connId) {
//     if (connectionsSignalSlot[connId]) {
//         connectionsSignalSlot[connId]()

//         setTimeout(() => {}, 0);
//     }
// }

// qnode.api.invoke = function(objId, methodName) {
//     return new Promise((resolve) => {
//         var reqId = $qnodeapi_invoke.apply(null, arguments)
//         invokeReturnCallbacks[reqId] = resolve
//     })
// }

// qnode.api.on = function(objId, signalName, callback) {
//     var connId = $qnodeapi_on(objId, signalName)
//     connectionsSignalSlot[connId] = callback
// }

// qnode.api.call = $qnodeapi_call
// qnode.api.methodList = function(objId) {
//     return qnode.api.call($qnodeapi_script_objects, "methodList(uint)", objId)
// }
// qnode.api.reflect = function(objId) {
//     var meta = {
//         name: qnode.api.call($qnodeapi_script_objects, "className(uint)", objId),
//     }
//     eval("meta.methods = " + qnode.api.call($qnodeapi_script_objects, "methodList(uint)", objId))
//     return meta
// }

qnode.QtObjectWrapper.qtClassMetaCache = {}

qnode.QtObjectWrapper.prototype.wrapMethods = function() {

    if(!qnode.QtObjectWrapper.qtClassMetaCache[this.className]) {
        qnode.QtObjectWrapper.qtClassMetaCache[this.className] = {
            methods: eval(this.methodList())
        }
    }

    var qtClassMeta = qnode.QtObjectWrapper.qtClassMetaCache[this.className]

    // console.log(this.methodList())

    for (var metaMethod of qtClassMeta.methods) {
        this[metaMethod.name] = ((signature) => {
            return function() {
                arguments.__proto__ = Array.prototype
                arguments.unshift(signature)
                return this.invoke.apply(this, arguments)
            }
        })(metaMethod.signature)
    }
}

// console.log(1)
// var qtClassMeta = qnode.reflect("BrowserWindow")
// console.log(2)
// console.log(qtClassMeta)

qnode.QtObjectWrapper.prototype.__proto__ = EventEmitter.prototype