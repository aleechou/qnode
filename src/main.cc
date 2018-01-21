#include "init.h"

void Init(v8::Local<v8::Object> exports) {
    QNodeInit(exports);
}

NODE_MODULE(qnode, Init)
