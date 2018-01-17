/*
TODO:
目前还没有处理 local_objects_by_hash_id 内存泄漏的问题，
当对象不再使用时，仍然被 local_objects_by_hash_id 引用
*/


var isOrigin = typeof process !== 'undefined' && process.versions && process.versions.node

// nodejs 环境
if (isOrigin) {
    var qnode = require("../../.bin/qnode.node")
}
// 浏览器环境
else {
    window.qnode = {}
}


var local_objects_by_hash_id = {}
var local_objects_hash_seed = 0;

function genHashId() {
    return (isOrigin ? "" : $window.id) + ":" + (local_objects_hash_seed++)
}

qnode.bridge = {}

qnode.bridge.object = function(object) {

    var meta = {
        bridgeTo: []
    }

    return proxy(object, meta, [])
}


function proxy(target, meta, path) {

    if (target.__isProxy) {
        throw new Error("can not pass a Proxy object as a proxy target")
    }

    // 新的 local id
    if (target["#"] === undefined) {
        target["#"] = genHashId()
        local_objects_by_hash_id[target["#"]] = 1 // 先占位
    }

    for (var name in target) {
        target[name] = propValue(name, target[name])
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
        set: function(_, prop, value) {

            // 没有变化
            if (target[prop] === value) {
                return true
            }

            // 更新数值
            value = propValue(prop, value)
            target[prop] = value

            // 同步桥接的对象
            asyncUpdateToBridges(prop, value)

            // 触发事件
            if (changeCallbacks[prop]) {
                for (var id in changeCallbacks[prop]) {
                    setImmediate(changeCallbacks[prop][id], value, prop, path, target["#"])
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
                return target["#"]
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

    // if(local_objects_by_hash_id[target["#"]] && local_objects_by_hash_id[target["#"]]!==1 && target["#"].substr(0,1)==qnode.api.threadId) {
    //     console.error("repeat object id", target["#"], target)
    //     console.error(local_objects_by_hash_id)
    // }
    local_objects_by_hash_id[target["#"]] = proxyobj


    function propValue(prop, value) {
        if ("object" == typeof value) {
            if (value.__isProxy === true)
                return value
                    // 普通对象
            if (value.constructor != Array)
                return proxy(value, meta, path.concat([prop]))
                    // 数组
            else {
                for (var i = 0; i < value.length; i++)
                    value[i] = propValue("#index", value[i])
                return proxyArray(prop, value)
            }
        } else
            return value

    }

    function proxyArray(prop, array) {
        array.push = function(...args) {

            for (var i = 0; i < args.length; i++)
                args[i] = propValue("#index", args[i])

            var newArray = this.concat(args)

            asyncUpdateToBridges(prop, newArray) // 同步桥接的对象
            target[prop] = proxyArray(prop, newArray) // 更新数组
        }
        array.splice = function(idx, length, ...replace) {
            if (length === undefined)
                length = 1

            for (var i = 0; i < replace.length; i++)
                replace[i] = propValue("#index", replace[i])

            var newArray = this.slice(0, idx)
                .concat(replace)
                .concat(this.slice(idx + length))
            asyncUpdateToBridges(prop, newArray) // 同步桥接的对象
            target[prop] = proxyArray(prop, newArray) // 更新数组
        }
        array.__isProxy = true
        return array
    }

    function asyncUpdateToBridges(prop, value) {

        // 变化来源
        if (__mutation_for === target["#"]) {
            return
        }

        // 同步桥接的对象
        if (isOrigin) {
            for (var window of meta.bridgeTo) {
                window.run(
                    () => {
                        qnode.bridge.commit(objectId, prop, value)
                    }, {
                        objectId: target["#"],
                        prop,
                        value
                    }
                )
            }
        } else {
            $window.runInNode(
                () => {
                    $$qnode$$.bridge.commit(objectId, prop, value)
                }, {
                    objectId: target["#"],
                    prop,
                    value
                }
            )
        }
    }

    return proxyobj
}

__mutation_for = undefined

qnode.bridge.commit = function(objectId, prop, value) {

    var object = local_objects_by_hash_id[objectId]

    if (!object) {
        console.error("unknow object for qnode.bridge.commit()", arguments)
        return;
    }

    __mutation_for = objectId

    object[prop] = value

    __mutation_for = undefined
}