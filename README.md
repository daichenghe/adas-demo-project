# adas_demo_project
a demo project you can get more acquainted with adas (some sensitive function body was hidden)
# **软件环境**
###     linux c多线程编程
###     source insight作为编辑器
###     gcc工具链编译
对应文件为 Makefile
###     图形库opengl

# **file structure 文件结构**
### avm_app 系统应用程序，隐去敏感代码及敏感函数的函数体
### avm_bsp 板载底层程序
### AVM_lib 算法及库接口
### avm_line 画线程序
### common 公用相关程序
### log log日志
### tool 第三方开源工具
### lut 标定数据
### media 贴图文件
### shaders 着色器程序
**以上程序未完全开放，但可以一窥程序之框架**

# **功能**
### AVM 全景监控影像系统
### bsd 盲区检测
### ldws 车道偏离报系统
### dvr 录制播放
### 行车轨迹线 wheel path

# **方案实现**
### demo基于opengl语言显示界面，使用摄像头标定结果通过坐标系、投影、矩阵变化处理相机数据并显示。
### 基于标定结果构画倒车线等行车轨迹线
### 基于算法识别车道线并向车辆提出预警
### 基于算法识别盲区物体并向车辆提出报警
### 通过usart与车辆通讯及交互切换画面并提供报警信息
