# bilidm

用于Arduino esp32的B站直播间弹幕监听库。（esp其他芯片应该也能直接使用）

## 安装
下载项目的压缩文件，使用Arduino IDE的Add .ZIP Library
## 用法
见examples
## 其他说明
项目代码参考了[blivedm](https://github.com/xfgryujk/blivedm)和[wiliwili](xfangfang.github.io/wiliwili)。直接包含了[brotli](https://github.com/google/brotli)代码用于解压。
目前B站的情况是没有登录的用户一段时间后会看不到弹幕的发送者的完整用户名和UID，现在等别人提供解决方案。
