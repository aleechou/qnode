qnode = {
    api: {
        threadId: $qnodeapi_thread_id,
        parentThreadId: $qnodeapi_parent_thread_id,
        consolePort: $qnodeapi_console_port
    },
    bridge: {},
    classes: {}
}

qnode.api.runScriptInThread = function(threadObjId, script) {
    return $window.runScriptInThread(threadObjId, script)
}
