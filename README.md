# JJfly-Quadrotor

本科期间参加电赛做的无人机项目，大概是14年开始做的，前后花了2年多时间，半个本科时光都献给这个项目了，从代码到PCB一点点建立完善，机架也是请朋友设计切割的。当时还想着做整套资料包出售赚钱来着，因此还拍了讲解视频，可惜后来不了了之。偶然间翻到上传，虽然貌似近几年电赛不太爱做飞行器了，而且这方面的开源资料也越来越多了，还是分享一下，供大家学习参考。代码比较烂，见谅。

简单介绍一下飞控（详细的介绍和演示可以观看视频）：

方案：主控tm4c123gh6pm，从控msp430g2553（主要负责读取光流传感器），摄像头控制器k60

支持接口：山外摄像头（智能车用的那个）、pxi光流、APM光流、各种超声波模块

实现功能：稳飞、定高、光流定点、摄像头寻迹（视频里面有展示）

![pic1.jpg](https://github.com/HuaxinLu/JJfly-Quadrotor/blob/master/%E5%AE%9E%E7%89%A9%E7%85%A7%E7%89%87/pic1.jpg) 
![pic1.jpg](https://github.com/HuaxinLu/JJfly-Quadrotor/blob/master/%E5%AE%9E%E7%89%A9%E7%85%A7%E7%89%87/pic2.jpg) 
