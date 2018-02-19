function loadScript(url) {
    return new Promise((resolve) => {
        var script = document.createElement('script');
        script.onload = resolve;
        script.onerror = console.error;
        script.src = url;
        document.body.appendChild(script)
    })
}

async function loadScriptList(lst) {
    for (var url of lst) {
        await loadScript(url)
    }
}

function envVarValue() {
    return new Promise((resolve) => {
        $window.envVarValue("QNODE_SDKPATH", resolve)
    })
}


;
(async() => {

    // 加载 sdk
    await loadScriptList([
        "qrc:/qtwebchannel/qwebchannel.js"
    ])

    // 初始化 qt webchannel
    new QWebChannel(qt.webChannelTransport, async function(channel) {

        for (var name in channel.objects) {
            window[name] = channel.objects[name]
        }

        var sdkPath = await envVarValue("QNODE_SDKPATH")

        // 加载剩余 sdk
        await loadScriptList([
            "file://" + sdkPath + "/browser/eventemitter.js",
            "file://" + sdkPath + "/browser/md5.js",
            "file://" + sdkPath + "/browser/window.js",
            "file://" + sdkPath + "/common/bridge.js"
        ])

        $window.nativeWindowId((id) => {
            $window.id = id
            $window.onLoaded()
        })

        qt.createObject = createQtObject

        qt.promisify = function(func, context) {
            return function() {
                return new Promise((resolve) => {
                    arguments[arguments.length++] = resolve
                    func.apply(context, arguments)
                })
            }
        }

        qt.clipboard = await qt.createObject("Clipboard")
        qt.clipboard.text = qt.promisify(qt.clipboard.text)

        qt._loadedlisteners.forEach((cb) => {
            try {
                cb()
            } catch (e) {
                console.error(e)
            }
        })
    })
})()

function createQtObject(className) {
    className = className.trim()
    if (className[className.length - 1] != "*")
        className += "*"
    return new Promise((resolve) => {
        $window.createQtObject(className, (object) => {

            object.on = function(signal, func) {
                this[signal].connect(func)
            }

            resolve(object)
        })
    })
}