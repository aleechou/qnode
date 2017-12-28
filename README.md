### 编译步骤

0. 进入 build 目录
```
cd build
```

1. 生成 Makefile
```
qmake ..
```

2. 生成 `binging.gyp` 文件
```
../tools/make-binding.js
```

3. 使用 node-gyp 编译成 nodejs 模块
```
node-gyp rebuild
```

4. 拷贝依赖的 qt 动态链接库
```
../tools/pack-qt.js
```


### 常见问题

1. 命令行 `qmake` 的版本需要和 `pkg-config` 找到得 Qt 一致. 安装了多个版本Qt时,可能会出现不一致的情况, 注意使用正确的那一个 qmake .

    如果不一致, 则会在编译时出现错误:

    ```
    This file was generated using the moc from 5.9.0. It
    cannot be used with the include files from this version of Qt.
    (The moc has changed too much.)
    ```

    qmake 生成的 *.moc 文件开头会检查 qmake 的版本.


2. 由脚本 `tools/make-binding.js` 生成的 `binding.gyp` 和 脚本 `tools/pack-qt.js` 均依赖 `pkg-config` 查找 Qt ,
如果 `pkg-config` 无法定位到正确版本的 Qt ,则构建回出错.

    可以通过环境变量 `PKG_CONFIG_PATH` 指定 Qt pc 文件的位置:
    ```
    export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:<Qt安装目录>/5.9/gcc_64/lib/pkgconfig/
    ```
