# 新大陆实验平台模块驱动newlab-stm32-driver  
1. 本项目是针对新大陆实验平台，对个模块独立编写的驱动程序代码库，有完整的接线图，可以直接使用测试模块好坏。  

2. 本项目中内容仅供学习使用，产生的不良后果，作者不负任何责任，请知悉。  

# 一、搭建Stm32开发环境  
1. 下载安装串口驱动 [点击下载]()  
当然对自己焊接水平自信的可以选择引出jtag调试口，使用jlink、stlink等仿真器下载程序。  

2. 下载串口烧写程序  
这里有两种，一种是新大陆推荐的程序，另一种是我推荐的程序。  
[点击下载(推荐)]()  
[点击下载(新大陆)]()

2. 下载安装keil for arm v5 [点击下载]()  

3. 破解keil

4. 安装设备支持包、固件库 [点击下载]()

# 二、新建项目
这里以串口实验为例。

1. 