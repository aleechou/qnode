const qnode = require(".")

global.$store = qnode.bridge.object({
    a: 1,
    b: 2,
    c: {
        aaa: 1,
        bbb: 2
    }
})

var mainWindow = new qnode.Window();
(async() => {

    await mainWindow.aload(__dirname + "/../sdk/example/index.html")

    var c = await mainWindow.arun(() => {
        var c = a + b
        setTimeout(() => resolve(c), 0)
    }, { a: 1, b: 2 })

    mainWindow.bridgeObject($store, ($store) => {
        window.$store = $store
    })

})()
mainWindow.show()


qnode.openDevConsole()


const repl = require('repl')
const msg = 'message'
global.qnode = qnode
repl.start('> ').context.m = msg