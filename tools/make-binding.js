#!/usr/bin/env node

const fs = require("fs")
const child_process = require("child_process")
const pt = require("path")



module.exports = function(bindingPath, cflags, funcMakeBinding) {

    if (!bindingPath) {
        bindingPath = process.cwd() + "/binding.gyp"
    }
    var bingdingDir = pt.dirname(bindingPath)

    // 生成 uic
    child_process.execFileSync("make", ["compiler_uic_make_all"])

    // 生成 rcc
    child_process.execFileSync("make", ["compiler_rcc_make_all"])

    // 生成 moc
    var output = child_process.execFileSync("make", ["compiler_moc_header_make_all"])

    // 将 .pro 文件里的 source 文件写入到 binding.gyp
    var fileBindingGyp = require(__dirname + "/binding.gyp.json")

    var sources = parseCppFilesFromMakefile(bingdingDir + "/Makefile")

    sources.forEach((file) => {
        fileBindingGyp.targets[0].sources.push(pt.relative(bingdingDir, file))
    })

    // 加入 qtsignalrouter.cc 文件
    if (fs.existsSync(bingdingDir + "/qtsignalrouter.cc")) {
        fileBindingGyp.targets[0].sources.push("./qtsignalrouter.cc")
    }

    console.log(fileBindingGyp.targets[0].sources)

    if (cflags) {
        fileBindingGyp.targets[0].cflags = (fileBindingGyp.targets[0].cflags || []).concat(cflags)
    }

    // qt libs
    switch (process.platform) {
        case 'darwin':
            // for macOS framework headers
            var qtpath = child_process.execFileSync("qmake", ["-query", "QT_INSTALL_PREFIX"]).toString().trim()
            var qtIncludesDir = bingdingDir + "/qtincludes"
            if (!fs.existsSync(qtIncludesDir)) {
                child_process.execFileSync('mkdir', ['-p', qtIncludesDir])
            }
            fileBindingGyp.targets[0].include_dirs.push(qtIncludesDir)

            // links libs
            fileBindingGyp.qtlibs.forEach((libname) => {
                fileBindingGyp.targets[0].conditions[1][1].libraries.push("-framework " + libname)

                var libHeadersDir = qtpath + "/lib/" + libname + ".framework/headers"
                fileBindingGyp.targets[0].include_dirs.push(libHeadersDir)

                if (!fs.existsSync(qtIncludesDir + '/' + libname)) {
                    child_process.execFileSync('ln', ['-s', libHeadersDir, qtIncludesDir + '/' + libname])
                }
            })
            break
        case 'linux':
            break
        default:
            throw new Error("unknow platform " + process.platform, )
    }

    if (funcMakeBinding) {
        funcMakeBinding(fileBindingGyp)
    }

    fs.writeFileSync(bindingPath, JSON.stringify(fileBindingGyp, null, 4))
}

function parseCppFilesFromMakefile(makefilePath) {
    var sources = []
    var qtdir = pt.dirname(makefilePath)
    var proFileContent = fs.readFileSync(makefilePath).toString()

    proFileContent = proFileContent
        .replace(/\\\r\n/g, ":")
        .replace(/\\\n/g, ":")

    var res = proFileContent.match(/SOURCES\s*\+?=([^\r\n]+)/)
    if (res) {

        var sourcesContent = res[1]
        sourcesContent = sourcesContent
            .replace(/\\ /g, "##space##")
            .replace(/ +/g, ":")
            .replace(/##space##/g, "\\ ")

        var files = sourcesContent.split(":")
        files.forEach((file, i) => {
            file = file.trim()
            if (!file)
                return

            file = file.replace(/\$\$PWD/g, '')

            var srccpp = pt.normalize(qtdir + "/" + file)
            if (!sources.includes(srccpp))
                sources.push(srccpp)
        })
    }

    return sources
}



if (process.argv[1] == __filename) {
    module.exports()
}