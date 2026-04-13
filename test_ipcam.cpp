// Create a test file test_ipcam.cpp
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::VideoCapture cap("https://10.35.20.50:8080/video");
    if (!cap.isOpened()) {
        std::cout << "Cannot open stream!" << std::endl;
        return -1;
    }
    
    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        
        cv::imshow("Phone Camera", frame);
        if (cv::waitKey(30) >= 0) break;
    }
    return 0;
}