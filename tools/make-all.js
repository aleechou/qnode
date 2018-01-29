#!/usr/bin/env node

const fs = require("fs")
const child_process = require("child_process")
const pt = require("path")


function exec(cmd, ...args) {
    return new Promise((resolve) => {
        var chdproc = child_process.execFile(cmd, args);
        chdproc.on('exit', resolve)
        chdproc.on('error', console.error)
        chdproc.stderr.pipe(process.stderr)
        chdproc.stdout.pipe(process.stdout)
    })
}

module.exports = async function(qtpro, targetQnode, buildDir, nativeClasses) {
    var cflags = []
    try {

        if (!qtpro)
            qtpro = __dirname + "/../qnode.pro"
        if (!targetQnode)
            targetQnode = __dirname + "/../.bin/qnode.node"
        if (!buildDir)
            buildDir = process.cwd()

        mkdir(buildDir)

        var signalRouterFile = buildDir + "/qtsignalrouter.cc"

        // 生成 qt 信号连接 c++代码
        // if (fs.existsSync(targetQnode)) {
        //     require("./make-qtclass-signals")(require(targetQnode), signalRouterFile, nativeClasses || [])
        //     cflags.push("-DQT_SIGNAL_ROUTER_FILE=\"" + signalRouterFile + "\"")
        // }

        var oricwd = process.cwd()
        process.chdir(buildDir)

        // qmake 
        await exec("qmake", "CONFIG+=release", qtpro)

        // 生成 bingding.gyp
        require("./make-binding.js")(qtpro, buildDir + "/binding.gyp", cflags)

        // 编译
        console.log("build qnode by node-gyp")
        await exec("node-gyp", "--release", "rebuild")

        // 打包
        await require("./pack-qt.js")(targetQnode, buildDir + "/build/Release/qnode.node")


        process.chdir(oricwd)

    } catch (e) {
        console.error(e)
    }

    console.log("done")
    process.exit()
}


function mkdir(path) {
    try {
        fs.mkdirSync(path)
    } catch (error) {}
}

if (process.argv[1] == __filename) {
    module.exports()
}