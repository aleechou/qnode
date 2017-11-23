var remote_objects_by_id = {}

var local_objects_by_id = {}
var local_objects_signned_id = 0


qnode.bridge.object = function(object, bridgeFrom, remoteId) {

    var meta = {
        bridgeFrom: bridgeFrom,
        remoteId: remoteId,
        id: local_objects_signned_id++,
        bridgeTo: [],
        isOrigin: bridgeFrom == undefined,
        isShadow: bridgeFrom != undefined,
    }

    if (meta.isShadow) {
        meta.bridgeTo.push([bridgeFrom, remoteId])
    }

    var localbject = proxy(object, meta, [])
    local_objects_by_id[meta.id] = localbject

    return localbject
}

qnode.bridge.bridgeTo = async function(threadId, object, funcReceiverInWindow) {
    if (!funcReceiverInWindow)
        funcReceiverInWindow = () => {}

    var remoteId = await qnode.api.run(threadId, () => {

        shadowObject = qnode.bridge.object(object, from, originId)

        resolve(shadowObject["@"].id)

        funcReceiverInWindow(object)

    }, { originId: object["@"].id, object, funcReceiverInWindow })

    object["@"].bridgeTo.push([threadId, remoteId])

    return remoteId
}

var proxyid_signned = 0


function proxy(target, meta, path) {

    if (target.__isProxy) {
        throw new Error("can not pass a Proxy object as a proxy target")
    }

    var proxyid = proxyid_signned++;

    for (var name in target) {
        if ("object" == typeof target[name] && target[name].constructor != Array) {
            target[name] = proxy(target[name], meta, path.concat([name]))
        }
    }

    var callbackid_signned = 0
    var changeCallbacks = {}
    var onchange = (prop, callback) => {
        if (!changeCallbacks[prop])
            changeCallbacks[prop] = {}
        var callbackid = callbackid_signned++;
        changeCallbacks[prop][callbackid] = callback
        return () => delete changeCallbacks[prop][callbackid]
    }

    var proxyobj = new Proxy(target, {
        set: function(target, prop, value) {

            // 没有变化
            if (target[prop] === value) {
                return true
            }

            // 同步桥接的对象
            for (var remote of meta.bridgeTo) {
                var threadId = remote[0]
                var remoteId = remote[1]

                // 变化来源
                if (qnode.bridge.__mutation_from === threadId && qnode.bridge.__mutation_for === remoteId) {
                    continue
                }
                qnode.api.run(threadId, () => {
                    qnode.bridge.commit(remoteId, path, prop, value, from)
                }, {
                    remoteId,
                    path,
                    prop,
                    value,
                    from: qnode.api.threadId
                })
            }


            if ("object" == typeof value && value.constructor != Array) {
                value = proxy(value, meta, path.concat([prop]))
            }

            target[prop] = value


            // 触发事件
            if (changeCallbacks[prop]) {
                for (var id in changeCallbacks[prop]) {
                    setImmediate(changeCallbacks[prop][id], value, prop, path, local_objects_by_id[meta.id])
                }
            }

            return true
        },
        get: function(target, prop) {
            if (prop == "@") {
                return meta
            } else if (prop == "*") {
                return target
            } else if (prop == "#") {
                return proxyid
            } else if (prop == '__isProxy') {
                return true
            } else if (prop == "change") {
                return onchange
            }

            // 这个函数 es6 尚未实现
            else if (prop == Symbol.toStringTag) {
                return function() {}
            }

            return target[prop]
        },
        deleteProperty: function(target, prop) {
            delete target[prop]
            return true
        },
        enumerate: function() {
            return Object.keys(target)
        }
    })
    return proxyobj
}

qnode.bridge.__mutation_from = undefined
qnode.bridge.__mutation_for = undefined

qnode.bridge.commit = function(localId, path, prop, value, from) {
    var localObject = local_objects_by_id[localId]
    if (!localObject) {
        console.error("unknow local object id for qnode.bridge.commit() ", localId)
        return
    }

    qnode.bridge.__mutation_from = parseInt(from)
    qnode.bridge.__mutation_for = parseInt(localId)

    var object = path.reduce((object, name) => object[name], localObject)

    object[prop] = value

    qnode.bridge.__mutation_from = undefined
    qnode.bridge.__mutation_for = undefined
}