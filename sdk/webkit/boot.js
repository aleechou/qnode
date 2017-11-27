function loadScript(url) {
    return new Promise((resolve)=>{
        var script = document.createElement('script');
        script.onload = resolve ;
        script.src=url;
        document.body.appendChild(script)
    })
}

async function loadScriptList(lst){
    for(var url of lst) {
        await loadScript(url)
    }
}


; (async ()=>{

    // 加载 sdk
    await loadScriptList([
        "qrc:/qtwebchannel/qwebchannel.js",
        "qrc:/sdk/webkit/api.js"
    ])

    // 初始化 qt webchannel
    new QWebChannel(qt.webChannelTransport, async function(channel) {

        for (var name in channel.objects) {
            window[name] = channel.objects[name]
        }

        // 加载剩余 sdk
        await loadScriptList([
            "qrc:/sdk/webkit/eventemitter.js",
            "qrc:/sdk/webkit/window.js",
            "qrc:/sdk/webkit/md5.js",
            "qrc:/sdk/common/api.run.js",
            "qrc:/sdk/common/bridge.js"
        ])

        qnode.window = $window 
        $window.onLoaded()
    
    })
})()