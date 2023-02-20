# 添加当前目录下所有源文件
aux_source_directory(. LIB_SRC)
# 当前目录下的文件生成一个链接库
add_library(frame ${LIB_SRC})