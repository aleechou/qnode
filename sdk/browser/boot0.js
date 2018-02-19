qt._loadedlisteners = []
qt.onLoaded = function(callback) {
    qt._loadedlisteners.push(callback)
}