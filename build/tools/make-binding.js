#!/usr/bin/env node

const fs = require("fs")
const child_process = require("child_process")
const pt = require("path")

// 执行 make 

// 生成 uic
child_process.execFileSync("make", ["compiler_uic_make_all"])

// 生成 rcc
child_process.execFileSync("make", ["compiler_rcc_make_all"])

// 生成 moc
var output = child_process.execFileSync("make", ["compiler_moc_header_make_all"])

// 将 .pro 文件里的 source 文件写入到 binding.gyp
var proFileContent = fs.readFileSync(__dirname + "/../../qnode.pri").toString()

proFileContent = proFileContent
    .replace(/\\\r\n/g, ":")
    .replace(/\\\n/g, ":")

var res = proFileContent.match(/SOURCES\s*\+?=([^\r\n]+)/)
if (res) {

    var fileBindingGyp = require(__dirname + "/binding.gyp.json")

    var files = res[1].split(":")
    console.log(files)
    files.forEach((file, i) => {
        file = file.trim()
        if (!file)
            return

        fileBindingGyp.targets[0].sources.push("../" + file)

        var mocfilename = "moc_" + pt.parse(file).base

        var mocfile = __dirname + "/../output/Release/moc/" + mocfilename
        console.log(mocfile)
        if (fs.existsSync(mocfile)) {
            fileBindingGyp.targets[0].sources.push("output/Release/moc/" + mocfilename)
        }
    })

    fs.writeFileSync(__dirname + "/../binding.gyp", JSON.stringify(fileBindingGyp, null, 4))
}