// test.js
var qnode = require(__dirname + '/build/build/Debug/qnode.node');

console.log(process.argv)

qnode.hello()

var bw = new qnode.QtObjectWrapper("BrowserWindow*")
    // console.log(bw.methodList())
bw.invoke('load(QString)', "http://www.163.com")
bw.invoke('show()')


// var window = new addon.QtObjectWrapper("BrowserWindow*");
// window.invoke("show()");