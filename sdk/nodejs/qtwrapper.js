const EventEmitter = require('events')
const qnode = require("../../.bin/qnode.node")

const NativeQtObjectWrapper = qnode.QtObjectWrapper

// QtObjectWrapper 继承自 EventEmitter
NativeQtObjectWrapper.prototype.__proto__ = EventEmitter.prototype


class QtObjectWrapper extends NativeQtObjectWrapper {

    constructor(className) {
        super(className)
        EventEmitter.apply(this)

        this.className = className.replace(/\*/g, '').trim()

        this.wrapNativeMethods()
    }

    on(eventName, callback) {
        var signalMeta = this.signalsByName(eventName)

        // is a qt signal
        if (signalMeta) {
            eventName = signalMeta.signature // 用 qt函数签名做事件名, 例如: `ready(QString,bool)`

            // 连接 qt 信号
            if (!this.listenerCount(eventName)) {
                this.connectQtSignal(signalMeta.idx)
            }
        }

        return EventEmitter.prototype.on.call(this, eventName, callback)
    }

    once(eventName, callback) {
        var signalMeta = this.signalsByName(eventName)

        // is a qt signal
        if (signalMeta) {
            eventName = signalMeta.signature // 用 qt函数签名做事件名, 例如: `ready(QString,bool)`

            // 连接 qt 信号
            if (!this.listenerCount(eventName)) {
                this.connectQtSignal(signalMeta.idx)
            }

            return EventEmitter.prototype.once.call(this, eventName, function() {
                callback.apply(this, arguments)

                // setImmediate() 确保 once 完毕,移除 listener 以后, 再判断是否需要解除  qt 信号/槽 连接
                setImmediate(() => {
                    if (!this.listenerCount(eventName)) {
                        this.disconnectQtSignal(signalMeta.idx)
                    }
                })
            })
        }
        // 普通js事件
        else {
            return EventEmitter.prototype.once.call(this, eventName, callback)
        }
    }

    removeAllListener(eventName, callback) {
        var signalMeta = this.signalsByName(eventName)

        // is a qt signal
        if (signalMeta) {
            eventName = signalMeta.signature // 用 qt函数签名做事件名, 例如: `ready(QString,bool)`

            // 解除 qt 信号/槽 连接
            this.disconnectQtSignal(signalMeta.idx)
        }

        return EventEmitter.prototype.removeAllListener.call(this, eventName, callback)
    }
    removeListener(eventName, callback) {

        var signalMeta = this.signalsByName(eventName)

        // is a qt signal
        if (signalMeta) {
            eventName = signalMeta.signature // 用 qt函数签名做事件名, 例如: `ready(QString,bool)`

            setImmediate(() => {
                // 解除 qt 信号/槽 连接
                if (!this.listenerCount(eventName)) {
                    this.disconnectQtSignal(signalMeta.idx)
                }
            })
        }

        return EventEmitter.prototype.removeListener.call(this, eventName, callback)
    }

    nativeClassMeta() {

        if (QtObjectWrapper.cacheNativeMethodsOfClasses[this.className]) {
            return QtObjectWrapper.cacheNativeMethodsOfClasses[this.className]
        }

        var methodList = eval(this.methodList())
        var classMeta = { methods: {}, signals: {} }
        for (var methodMeta of methodList) {
            // signal
            if (methodMeta.type == 1) {
                classMeta.signals[methodMeta.name] = methodMeta
            }
            // qt invokable method
            else if (methodMeta.type != 3) { // 3=构造函数
                classMeta.methods[methodMeta.name] = methodMeta
            }
        }
        return QtObjectWrapper.cacheNativeMethodsOfClasses[this.className] = classMeta
    }

    wrapNativeMethods() {
        var classMeta = this.nativeClassMeta()
        for (var methodName in classMeta.methods) {
            this[methodName] = ((methodMeta) => function(...args) {
                return this.invoke(methodMeta.idx, ...args)
            })(classMeta.methods[methodName])
        }
    }

    signalsByName(signalName) {
        return this.nativeClassMeta().signals[signalName]
    }

    onQtSignalReceived(eventName, ...args) {
        this.emit(eventName, ...args)
    }
}

QtObjectWrapper.cacheNativeMethodsOfClasses = {}

qnode.QtObjectWrapper = QtObjectWrapper