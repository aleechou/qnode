#!/usr/bin/env node

const fs = require("fs")
const child_process = require("child_process")
const pt = require("path")

module.exports = function(qtpro, bindingPath) {

    if(!bindingPath) {
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
    if(qtpro)
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

    fs.writeFileSync(bindingPath, JSON.stringify(fileBindingGyp, null, 4))
}

function parseSourceCpp(qtpro, sources) {

    var qtdir = pt.dirname(qtpro)

    var proFileContent = fs.readFileSync(__dirname + "/../qnode.pri").toString()

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

            var srccpp = pt.normalize(qtdir + "/" + file)
            if(!sources.includes(srccpp))
                sources.push(srccpp)
        })
    }

    return sources
}



if( process.argv[1] == __filename ) {
    module.exports(  )
}