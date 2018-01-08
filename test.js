// test.js
var qnode = require(__dirname + '/build/build/Debug/qnode.node');

console.log(process.argv)

qnode.hello()

console.log("xxxx")
var bw = new qnode.QtObjectWrapper("BrowserWindow")
console.log("xxxx")

// var window = new addon.QtObjectWrapper("BrowserWindow*");
// window.invoke("show()");