#!/usr/bin/env node

const fs = require("fs")
const child_process = require("child_process")
const pt = require("path")



module.exports = function(qtpro, bindingPath, cflags) {

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

    var sources = parseSourceCpp(__dirname + "/../qnode.pri", [])
    if (qtpro)
        sources = parseSourceCpp(qtpro, sources)

    sources.forEach((file, i) => {

        fileBindingGyp.targets[0].sources.push(pt.relative(bingdingDir, file))

        var mocfilename = "moc_" + pt.parse(file).base

        var mocfile = "output/Release/moc/" + mocfilename
        if (fs.existsSync(mocfile)) {
            fileBindingGyp.targets[0].sources.push("output/Release/moc/" + mocfilename)
        }
    })

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
        default:
            throw new Error("unknow platform " + process.platform, )
    }
    delete fileBindingGyp.qtlibs

    fs.writeFileSync(bindingPath, JSON.stringify(fileBindingGyp, null, 4))
}

function parseSourceCpp(qtpro, sources) {

    var qtdir = pt.dirname(qtpro)

    var proFileContent = fs.readFileSync(qtpro).toString()

    proFileContent = proFileContent
        .replace(/\\\r\n/g, ":")
        .replace(/\\\n/g, ":")

    var res = proFileContent.match(/SOURCES\s*\+?=([^\r\n]+)/)
    if (res) {

        var files = res[1].split(":")
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
    // console.log(qtpro, sources)

    return sources
}



if (process.argv[1] == __filename) {
    module.exports()
}