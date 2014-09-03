#PointDownload

**点载,是一个能帮助你方便地从网络上下载各种文件的软件。现在已经支持主流的各种下载协议，包括迅雷的thunder://和QQ旋风的下载协议。
当前支持的操作系统有：LinuxDeepin2014, Ubuntu14.04, UbuntuKy14.04.
其他Linux发行版本的安装包暂时没有提供，因为开发和维护人员有限，如果您能帮助我们打包和维护其他Linux发行版本的安装包，请与我们联系！**


##开发指南

###开发环境：
Qt5.2.1
此项目共分为PointDownload、PointPopup、PointChromeExtension和PointFirefoxExtension4个模块，
未来可能还会增加或者删除其他模块以支持更多功能的需要。
其中，PointDownload是主程序部分，负责下载和管理的工作；
PointPopup是下载前配置弹出窗，负责接受浏览器插件传递的URL信息(或者点击新建按钮由用户填入URL信息),
并根据URL获取文件名、文件大小等信息显示，为用户确定下载做准备，
用户确定下载后将信息传递给主程序，由主程序进行下载；
PointChromeExtension模块请参考其中的README说明；
PointFirefoxExtension请参考其中的README说明。

###关于you-get的支持

本软件支持使用you-get下载在线视频网站的视频，
关于you-get的详细信息，请参考：[You-Get](https://github.com/soimort/you-get)。
you-get并不属于本程序的任何一部分，在接受you-get提供的服务的同时，请按照you-get作者的要求履行您应履行的义务。
安装you-get注意事项：
如果您不能按照[You-Get](https://github.com/soimort/you-get) 页面中描述的方法安装you-get，
请按照以下操作提示操作：

1.将you-get的源代码解压到任意路径下(请确保读写与执行权限),例如/home/Point/you-get-develop

2.$ cd /usr/local/bin/

3.$ ln -s /home/Point/you-get-develop/you-get you-get
