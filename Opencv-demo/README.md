# opencv_demo

> 工程定位：OpenCV5 官方教程配套学习Demo项目，用于分模块练习基础API；**独立工程，与C-Opencv5工具库解耦**，不引入业务工具代码。
> 构建方式：CMake + C++17

## 目录结构

opencv_demo/
├── README.md                    # 工程说明、编译步骤、OpenCV 版本、环境说明
├── assets/                      # 素材资源（图片、视频、标定图，不要放源码）
│   ├── images/
│   │   ├── lena.jpg
│   │   ├── test1.jpg
│   │   └── ...
│   └── videos/
│       └── test.mp4
├── src/                         # 所有 Demo 源码目录
│   ├── common/                  # Demo 共用辅助代码（少量，不要写复杂业务）
│   │   ├── demo_utils.h
│   │   └── demo_utils.cpp       # 窗口封装、图片加载、计时、打印辅助函数
│   ├── 01_core_basic/           # 【OpenCV 官方教程：Core 模块基础】
│   │   ├── 01_mat_create.cpp    # Mat 创建、初始化
│   │   ├── 02_mat_access.cpp    # Mat 像素访问
│   │   ├── 03_mat_arithmetic.cpp # 矩阵运算
│   │   └── CMakeLists.txt
│   ├── 02_imgproc/              # 【Imgproc 图像处理模块】
│   │   ├── 01_color_space.cpp    # 色彩空间转换 BGR/RGB/HSV/Gray
│   │   ├── 02_smooth.cpp         # 图像滤波（均值、高斯、中值）
│   │   ├── 03_threshold.cpp       # 阈值分割
│   │   ├── 04_edge.cpp           # Canny 边缘检测
│   │   └── CMakeLists.txt
│   ├── 03_highgui/              # 【HighGUI 图像窗口、读写、键鼠交互】
│   │   ├── 01_imread_imshow.cpp
│   │   ├── 02_trackbar.cpp
│   │   ├── 03_mouse_callback.cpp
│   │   └── CMakeLists.txt
│   ├── 04_videoio/              # VideoIO 视频读写、摄像头
│   │   ├── 01_camera_capture.cpp
│   │   ├── 02_video_read.cpp
│   │   └── CMakeLists.txt
│   ├── 05_features2d/           # 特征点（SIFT/ORB 等，官方进阶）
│   ├── 06_calib3d/              # 相机标定、立体视觉
│   └── 07_ml/                   # 机器学习模块（可选，后期练习）
├── build/                       # CMake 编译输出目录（git 忽略，不提交）
└── .gitignore                   # git 忽略规则：build、*.user、vs 缓存、*.log


## 设计约定
1. **单文件单main函数**
每个 `.cpp` 文件独立包含 `main()`，一个文件对应一个知识点Demo。CMake中可注释`add_executable`选择性编译，无需注释代码切换实验，贴合官方小Demo风格。

2. `src/common` 公共工具约束
仅存放Demo轻量辅助函数，**不移植主项目path_utils/mat_info**，保持项目隔离。
- 素材路径宏 `DEMO_ASSET(relpath)`
- 简易计时函数
- 窗口安全销毁、图片保存封装

3. CMake分层组织
- 顶层CMakeLists：查找OpenCV5、设置C++17、加载子目录
- 每个模块目录自带CMakeLists.txt，单独添加可执行目标
