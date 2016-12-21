基于预测与变换的图像压缩编码
---


按照纹理信息对图像块分割，针对不同的块类型，建立马尔可夫模型得到残差变换量化熵编码。

## file
src 源码
build 构建工程的目录
resources 使用的图片

## build
使用cmake构建工程

## config
配置系统保留堆栈大小为100000000

## run
使用如下命令行参数运行
```
LSOptimal.exe input.raw output.raw width height 0(little endian) 8(precision)
```
