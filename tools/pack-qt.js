#!/usr/bin/env node

const child_process = require("child_process")
const fs = require("fs")
const pt = require("path")
const util = require("util")

const copyFile = util.promisify(fs.copyFile)

async function pack4linux(targetQnode, buildQnode) {

    if (!targetQnode)
        targetQnode = __dirname + "/../.bin/qnode.node"
    var targetDir = pt.dirname(targetQnode)

    if (!buildQnode)
        buildQnode = process.cwd() + "/build/Release/qnode.node"
    buildDir = pt.dirname(buildQnode)

    console.log(buildQnode, "-->>", targetQnode)

    // qnode.node 文件从编译目录，拷贝到目标目录
    mkdir(targetDir)
    child_process.execFileSync("cp", [buildQnode, targetQnode])

    // 连续执行两遍,
    // 第一遍执行时,如果 qnode.node 找不到 libQt5Core.so.5 ,
    // 则用 ldd 查看依赖 libicui18n.so 的版本会是 55 , Qt5.9并未提供. 
    await packDeps()

    // 第二遍,在提供正确 libQt5Core 之后,
    // ldd 会返回依赖项 libicui18n.so.56
    await packDeps()


    async function packDeps() {

        console.log("copy deps:")

        // 查看并分析 qnode.node 的依赖项
        var libs = []
        var output = (child_process.execFileSync("ldd", ["-v", targetQnode])).toString()
            // console.log(output)

        var res = matchAll(/([^\s]+?) (\([^\)]+ \))?=> /g, output)
        for (var item of res) {
            if (libs.indexOf(item[1]) < 0) {
                libs.push(item[1])
            }
        }


        // 用 pkg-config 查找 qt安装目录
        ouput = (child_process.execFileSync("pkg-config", ["--libs-only-L", "Qt5Core"])).toString()
        var qtlib = ouput.trim().replace(/^\-L/, '')


        // 从 qt 的安装目录拷贝依赖
        // console.log(ouput)
        for (libfile of libs) {
            var libpath = qtlib + "/" + libfile
            if (!fs.existsSync(libpath)) {
                continue
            }

            var targetpath = targetDir + "/" + libfile

            try {
                child_process.execFileSync("cp", [libpath, targetpath])
            } catch (error) {}

            console.log(libpath, "=>", targetpath)
        }
    }
}

async function pack4darwin(targetQnode, buildQnode) {

    if (!targetQnode)
        targetQnode = __dirname + "/../.bin/qnode.node"
    var targetDir = pt.dirname(targetQnode)

    if (!buildQnode)
        buildQnode = process.cwd() + "/build/Release/qnode.node"
    buildDir = pt.dirname(buildQnode)

    console.log(buildQnode, "-->>", targetQnode)

    // qnode.node 文件从编译目录，拷贝到目标目录
    mkdir(targetDir)
    child_process.execFileSync("cp", [buildQnode, targetQnode])

    // mkdir Contents/Frameworks
    var frameworksDir = targetDir + "/../Contents/Frameworks"
    if (!fs.existsSync(frameworksDir)) {
        child_process.execFileSync('mkdir', ["-p", frameworksDir])
    }

    var qtpath = child_process.execFileSync("qmake", ["-query", "QT_INSTALL_PREFIX"]).toString().trim()


    var binding = require("./binding.gyp.json")
    console.log(binding)
    binding.qtlibs.forEach((libname) => {
        var dist = frameworksDir + "/" + libname + '.framework'
        if (!fs.existsSync(dist)) {
            var src = qtpath + '/lib/' + libname + '.framework'
            console.log("cp framework:", src, "->", dist)
            child_process.execFileSync('ln', ["-s", src, dist])
        }
    })
}

function mkdir(path) {
    try {
        fs.mkdirSync(path)
    } catch (error) {}
}

// console.log(libs)

function matchAll(regexp, text) {
    var ret = []
    var res
    while (res = regexp.exec(text)) {
        delete res.input
        ret.push(res)
    }
    return ret
}

if (process.platform == 'darwin')
    module.exports = pack4darwin
else
    module.exports = pack4linux


if (process.argv[1] == __filename) {
    module.exports()
}