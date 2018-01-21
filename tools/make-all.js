#!/usr/bin/env node

const fs = require("fs")
const child_process = require("child_process")
const pt = require("path")


function exec(cmd, ...args) {
    return new Promise((resolve) => {
        var chdproc = child_process.execFile(cmd, args);
        chdproc.on('exit', resolve)
        chdproc.stderr.pipe(process.stderr)
        chdproc.stdout.pipe(process.stdout)
    })
}

module.exports = async function(qtpro, targetQnode, buildDir) {

    if(!qtpro)
        qtpro = __dirname + "/../qnode.pro"
    if(!targetQnode)
        targetQnode = __dirname + "/../.bin/qnode.node"
    if(!buildDir)
        buildDir = process.cwd()

    var oricwd = process.cwd()
    process.chdir(buildDir)

    // qmake 
    await exec("qmake", "CONFIG+=release", qtpro)

    // 生成 bingding.gyp
    require("./make-binding.js") (qtpro, buildDir + "/binding.gyp")

    // 编译
    await exec("node-gyp", "--release", "rebuild")

    // 打包
    require("./pack-qt.js") ()


    process.chdir(oricwd)
}


if( process.argv[1] == __filename ) {
    module.exports(  )
}