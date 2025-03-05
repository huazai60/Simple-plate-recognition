#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>

using namespace cv;
using namespace std;

Mat preprocess_plate(Mat license_plate_region) {
    Mat gray, thresh;

    // 1. 转为灰度图
    cvtColor(license_plate_region, gray, COLOR_BGR2GRAY);

    // 2. 自适应阈值处理（适应不同底色的车牌）
    adaptiveThreshold(gray, thresh, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
                      THRESH_BINARY, 11, 2);

    // 3. 判断是否需要反色处理（适用于黄底黑字车牌）
    double mean_intensity = mean(thresh)[0];
    if (mean_intensity > 127) { // 如果大部分是白色，可能需要反色
        bitwise_not(thresh, thresh);
    }

    return thresh;
}

Mat detect_license_plate(Mat frame, tesseract::TessBaseAPI* ocr) {
    Mat gray, gray_filtered, edged;

    // 1. 转为灰度图
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // 2. 降噪并保留边缘：双边滤波
    bilateralFilter(gray, gray_filtered, 11, 17, 17);

    // 3. 边缘检测（Canny 算法）
    Canny(gray_filtered, edged, 30, 200);

    // 4. 查找轮廓
    vector<vector<Point>> contours;
    findContours(edged, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    sort(contours.begin(), contours.end(),
         [](const vector<Point>& c1, const vector<Point>& c2) {
             return contourArea(c1, false) > contourArea(c2, false);
         });

    vector<Point> plate_contour;
    string license_plate_text;

    // 5. 遍历轮廓，寻找近似四边形区域
    for (size_t i = 0; i < min(contours.size(), size_t(10)); i++) {
        double perimeter = arcLength(contours[i], true);
        vector<Point> approx;
        approxPolyDP(contours[i], approx, 0.018 * perimeter, true);

        if (approx.size() == 4) {  // 近似为四边形
            plate_contour = approx;
            Rect rect = boundingRect(approx);
            Mat license_plate_region = frame(rect);

            // 6. 预处理车牌区域
            Mat license_plate_thresh = preprocess_plate(license_plate_region);

            // 7. OCR 识别
            ocr->SetImage(license_plate_thresh.data, license_plate_thresh.cols,
                          license_plate_thresh.rows, 1, license_plate_thresh.step);
            char* outText = ocr->GetUTF8Text();
            license_plate_text = string(outText);
            delete[] outText;

            // 8. 去除无效字符（如标点符号）
            license_plate_text.erase(remove_if(license_plate_text.begin(), license_plate_text.end(),
                                               [](char c) {
                                                   return !isalnum(c);  // 仅保留字母和数字
                                               }),
                                     license_plate_text.end());

            break;  // 找到一个候选区域后退出
        }
    }

    // 9. 绘制检测到的车牌区域和识别结果
    if (!plate_contour.empty()) {
        polylines(frame, plate_contour, true, Scalar(0, 255, 0), 3);
        Rect rect = boundingRect(plate_contour);
        putText(frame, license_plate_text, Point(rect.x, rect.y - 10),
                FONT_HERSHEY_SIMPLEX, 0.9, Scalar(0, 255, 0), 2);
    }

    return frame;
}

int main() {
    // 打开默认摄像头
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "无法打开摄像头！" << endl;
        return -1;
    }

    // 初始化 Tesseract OCR
    tesseract::TessBaseAPI ocr;
    if (ocr.Init(NULL, "eng", tesseract::OEM_LSTM_ONLY)) {
        cerr << "无法初始化 Tesseract OCR 引擎！" << endl;
        return -1;
    }

    ocr.SetPageSegMode(tesseract::PSM_SINGLE_LINE); // 类似于 --psm 7
    ocr.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"); // 限制字符范围

    // 创建可调整大小的窗口
    namedWindow("车牌识别 - 视频流", WINDOW_NORMAL);
    resizeWindow("车牌识别 - 视频流", 800, 600);

    while (true) {
        Mat frame;
        cap >> frame;  // 读取视频帧
        if (frame.empty()) {
            cerr << "无法读取视频帧！" << endl;
            break;
        }

        // 检测车牌并识别字符
        Mat processed_frame = detect_license_plate(frame, &ocr);

        // 显示结果
        imshow("车牌识别 - 视频流", processed_frame);

        // 按 ESC 键退出
        if (waitKey(1) == 27) {
            break;
        }
    }

    cap.release();
    destroyAllWindows();
    ocr.End();

    return 0;
}
