var child_process = require("child_process")
var fs = require("fs")

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









// console.log(libs)

function matchAll(regexp, text) {
    var ret = []
    var res
    while (res = regexp.exec(output)) {
        delete res.input
        ret.push(res)
    }
    return ret
}