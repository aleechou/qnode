#!/usr/bin/env node

const fs = require("fs")
const child_process = require("child_process")
const pt = require("path")
const qtSignalsRouterMaker = require("./make-qtclass-signals")


function exec(cmd, ...args) {
    return new Promise((resolve) => {
        var chdproc = child_process.execFile(cmd, args);
        chdproc.on('exit', resolve)
        chdproc.on('error', console.error)
        chdproc.stderr.pipe(process.stderr)
        chdproc.stdout.pipe(process.stdout)
    })
}

function getCmdlineArgv(name) {
    var nameIndex = process.argv.findIndex(arg => arg == name)
    if (nameIndex > 0) {
        return process.argv[nameIndex + 1]
    }
}

module.exports = async function(qtpro, targetQnode, buildDir, nativeClasses, funcMakeBinding) {

    var bRebuild = process.argv.includes('--rebuild')

    if (!nativeClasses) {
        nativeClasses = {}
    }

    // 来自命令行的 nativeClasses
    var argNativeClassesValue = getCmdlineArgv("--nativeClasses")
    if (argNativeClassesValue != undefined) {
        try {
            argNativeClassesValue = JSON.parse(argNativeClassesValue)
        } catch (e) {
            console.error(e)
        }
        if (argNativeClassesValue) {
            nativeClasses = Object.assign(nativeClasses, argNativeClassesValue)
        }
    }

    var cflags = []
    try {

        if (!qtpro)
            qtpro = __dirname + "/../qnode.pro"
        var argvTarget = getCmdlineArgv("--target")
        if (argvTarget) {
            targetQnode = argvTarget
        }
        if (!targetQnode)
            targetQnode = __dirname + "/../.bin/qnode.node"

        if (!buildDir)
            buildDir = process.cwd()

        if (!fs.existsSync(buildDir)) {
            bRebuild = true
        }

        mkdir(buildDir)

        var signalRouterFile = buildDir + "/qtsignalrouter.cc"
            // cflags.push("-DQT_SIGNAL_ROUTER_FILE=\"" + signalRouterFile + "\"")

        // 生成 qt 信号连接 c++代码
        if (process.argv.includes('--signals')) {
            if (fs.existsSync(targetQnode)) {
                qtSignalsRouterMaker(require(targetQnode), signalRouterFile, nativeClasses || [])
                process.exit()
            }
        }

        // 清空 qtsignalrouter.cc 文件
        if (bRebuild && fs.existsSync(signalRouterFile)) {
            fs.unlinkSync(signalRouterFile)
        }
        if (!fs.existsSync(signalRouterFile)) {
            fs.writeFileSync(signalRouterFile, "")
            var remakeQtSignalRouter = true
        }

        var oricwd = process.cwd()
        process.chdir(buildDir)

        // qmake 
        if (bRebuild || process.argv.includes('--qmake')) {
            await exec("qmake", "CONFIG+=release", qtpro)
        }

        // 生成 bingding.gyp
        var bBinding = process.argv.includes('--binding')
        if (bRebuild || bBinding) {
            require("./make-binding.js")(buildDir + "/binding.gyp", cflags, funcMakeBinding)

            if (bBinding) {
                process.exit()
            }
        }

        // 编译
        if (!process.argv.includes('--noMake')) {
            console.log("build qnode by node-gyp")
            await exec("node-gyp", "--release", bRebuild ? "rebuild" : "build")
        }

        // 打包
        if (bRebuild) {
            await require("./pack-qt.js")(targetQnode, buildDir + "/build/Release/qnode.node")
        } else {
            console.log("cp", buildDir + "/build/Release/qnode.node", "-->>", targetQnode)
            fs.copyFileSync(buildDir + "/build/Release/qnode.node", targetQnode)
        }

        // 无 qtsignalrouter.cc 文件编译
        // 则在编译后生成 qtsignalrouter.cc 文件, 并再次编译
        // (qtsignalrouter.cc 需要在运行时反射生成)
        if (remakeQtSignalRouter) {
            console.log("make qtsignalrouter.cc")
            await exec(process.argv[0], __filename, "--signals", "--nativeClasses", JSON.stringify(nativeClasses), "--target", targetQnode)

            console.log("build qnode again")
            await exec("node-gyp", "--release", "build")
        }

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