# bilidm

用于Arduino esp32的B站直播间弹幕监听库。（esp其他芯片应该也能直接使用）



## 安装
下载项目的压缩文件，使用Arduino IDE的Add .ZIP Library
## 用法
见examples
## 其他说明
项目代码参考了[blivedm](https://github.com/xfgryujk/blivedm)和[wiliwili](xfangfang.github.io/wiliwili)。直接包含了[brotli](https://github.com/google/brotli)代码用于解压。

B站在2023年7月左右开始对游客模式下的直播弹幕进行用户名打码、限流等操作。需要向b站申请开发者资格，或者使用cookie，目前没有实现。


## 一个示例：
[bot.webm](https://github.com/user-attachments/assets/33ec0812-bfbe-41fd-9f71-d2d312a1ea22)
