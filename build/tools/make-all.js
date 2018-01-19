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

;
(async() => {
    // qmake 
    exec("qmake", "CONFIG+=release", __dirname + "/../..")

    // 生成 bingding.gyp
    require("./make-binding.js")

    // 编译
    exec("node-gyp", "--release", "rebuild")
})()