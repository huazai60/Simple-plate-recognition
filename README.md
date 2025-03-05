# Plate Recognition | 车牌识别

A real-time license plate recognition system using **OpenCV** and **Tesseract OCR**. The program captures video from a webcam, detects a license plate in the frame, and extracts characters using OCR.

使用 **OpenCV** 和 **Tesseract OCR** 实现的实时车牌识别系统。该程序通过摄像头捕获视频，检测帧中的车牌，并使用 OCR 提取字符。

---

## Features | 功能特点
- Uses OpenCV for image processing (grayscale, thresholding, edge detection, contour detection)
- Uses Tesseract OCR to recognize license plate characters
- Detects and highlights license plates in real-time video stream
- Optimized for high accuracy and performance

- 使用 OpenCV 进行图像处理（灰度化、阈值处理、边缘检测、轮廓检测）
- 使用 Tesseract OCR 进行车牌字符识别
- 在实时视频流中检测并高亮显示车牌
- 针对高准确性和性能进行了优化

---

## Installation | 安装

### 1. Install Dependencies | 安装依赖

#### Ubuntu / Debian
```bash
sudo apt update
sudo apt install libopencv-dev tesseract-ocr libtesseract-dev libleptonica-dev cmake g++
```

#### Mac (Homebrew)
```bash
brew install opencv tesseract leptonica cmake
```

#### Windows (vcpkg)
```bash
vcpkg install opencv tesseract leptonica
```

### 2. Install Additional Tesseract Language (Optional) | 安装额外的 Tesseract 语言支持（可选）
If you need additional languages for OCR:
```bash
sudo apt install tesseract-ocr-chi-sim  # Chinese Simplified OCR
```
Or manually download `.traineddata` from [Tesseract Language Data](https://github.com/tesseract-ocr/tessdata) and place it in the `tessdata` directory.

---

## Compilation & Running | 编译与运行

### 1. Compile with g++ | 使用 g++ 编译
```bash
g++ -o plate_recognition plate_recognition.cpp `pkg-config --cflags --libs opencv4 tesseract`
```

### 2. Compile with CMake | 使用 CMake 编译
Create a `CMakeLists.txt` file:

创建 `CMakeLists.txt` 文件：
```cmake
cmake_minimum_required(VERSION 3.10)
project(plate_recognition)

find_package(OpenCV REQUIRED)
find_package(Tesseract REQUIRED)

add_executable(plate_recognition plate_recognition.cpp)
target_link_libraries(plate_recognition ${OpenCV_LIBS} Tesseract::Tesseract)
```
Then build and run:

然后进行构建和运行：
```bash
mkdir build && cd build
cmake ..
make
./plate_recognition
```

---

## Usage | 使用方法
Run the program:

运行程序：
```bash
./plate_recognition
```
The program will open a webcam feed, detect license plates, and overlay the recognized characters on the video stream.

程序会打开摄像头视频流，检测车牌，并在视频画面中叠加识别出的车牌字符。

---

## Example Output | 示例输出
If a license plate is detected, it will be outlined in green and the extracted text will be displayed above it.

如果检测到车牌，它会被绿色轮廓框住，并在上方显示识别出的文本。

![Example](example_output.png)

---

## License | 许可证
This project is licensed under the MIT License.

本项目采用 MIT 许可证。

---

## Acknowledgments | 致谢
- **OpenCV** - Open-source computer vision library
- **Tesseract OCR** - Optical character recognition engine
- **Leptonica** - Image processing library for Tesseract

- **OpenCV** - 开源计算机视觉库
- **Tesseract OCR** - 光学字符识别引擎
- **Leptonica** - Tesseract 依赖的图像处理库

---

## Contributing | 贡献
Pull requests and suggestions are welcome!

欢迎提交 Pull Request 或提出建议！

