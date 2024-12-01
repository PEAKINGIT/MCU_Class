# MCU_Class
BigProjectForMCUcLass

# 代码分析(需要解释的)
## TEXT目录下是中文显示有关内容
## MALLOC是手动实现内存分配malloc 直接用C的malloc容易让内存不够
## FATFS就是fat文件系统代码
## PICTURE为图片显示驱动代码

# 11.27代码整合
## 基本包含好了所有的驱动代码
## 显示部分函数拆分到了lcd_display.c和main_interface.c
## 主函数仅包含外设初始化函数和变量初始化
## 特定功能的全局变量声明均放在对应.h里

# 12.1
## 模块化代码 解耦
## 修正点原子代码的bug...