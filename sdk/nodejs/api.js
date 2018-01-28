const qnode = require(process.env.QNODEPATH || "../../.bin/qnode.node")
const path = require("path")

process.env.QNODE_SDKPATH = path.dirname(__dirname)

qnode.openDevConsole = function() {
    var win = new qnode.Window
    win.load("http://127.0.0.1:" + qnode.consolePort)
    win.show()
    return win
}

// v8 compile() 函数执行的js代码中没有 require() 函数
// 无法直接得到 qnode
global.$$qnode$$ = qnode


global.requireForBrowser = function(url) {
    if( url.substr(0,8) != 'require:' ){
        return `console.error("unknow url scheme for ${url}")`
    }
    url = url.substr(8)
    console.log(url)
    return "console.log(1)"
}