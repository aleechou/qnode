#!/usr/bin/env node

var child_process = require("child_process")
var fs = require("fs")


// 连续执行两遍,
// 第一遍执行时,如果 qnode.node 找不到 libQt5Core.so.5 ,
// 则用 ldd 查看依赖 libicui18n.so 的版本会是 55 , Qt5.9并未提供. 
proc()

// 第二遍,在提供正确 libQt5Core 之后,
// ldd 会返回依赖项 libicui18n.so.56
proc()


function proc() {
    // 查看并分析 qnode.node 的依赖项
    var libs = []
    var output = child_process.execFileSync("ldd", ["-v", __dirname + "/../build/build/Release/qnode.node"]).toString()
        // console.log(output)

    var res = matchAll(/([^\s]+?) (\([^\)]+ \))?=> /g, output)
    for (var item of res) {
        if (libs.indexOf(item[1]) < 0) {
            libs.push(item[1])
        }
    }

    // console.log(libs)


    // 用 pkg-config 查找 qt安装目录
    ouput = child_process.execFileSync("pkg-config", ["--libs-only-L", "Qt5Core"]).toString()
    var qtlib = ouput.trim().replace(/^\-L/, '')


    // 从 qt 的安装目录拷贝依赖
    try {
        fs.mkdirSync(__dirname + "/../build/Release/lib")
    } catch (error) {}
    // console.log(ouput)
    for (libfile of libs) {
        var libpath = qtlib + "/" + libfile
        if (!fs.existsSync(libpath)) {
            continue
        }

        var targetpath = __dirname + "/../build/build/Release/" + libfile

        try {
            fs.copyFileSync(libpath, targetpath)
        } catch (error) {}

        console.log(libpath, "=>", targetpath)
    }
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