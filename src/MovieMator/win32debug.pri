## 生成 pdb符号文件
#win32-g++ {
#    #加入调试信息
#    QMAKE_CFLAGS_RELEASE += -g
#    QMAKE_CXXFLAGS_RELEASE += -g
#    #禁止优化
#    QMAKE_CFLAGS_RELEASE -= -O2
#    QMAKE_CXXFLAGS_RELEASE -= -O2
#    #release在最后link时默认有"-s”参数，表示"Omit all symbol information from the output file"，因此要去掉该参数
#    QMAKE_LFLAGS_RELEASE =
#}

## 打包
win32-g++ {
    #禁止优化
    QMAKE_CFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE -= -O2
}
