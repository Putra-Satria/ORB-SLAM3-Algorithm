#include <opencv2/opencv.hpp>
#include "System.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: mono_live vocabulary.yaml camera.yaml" << std::endl;
        return -1;
    }

    std::string vocab = argv[1];
    std::string camSettings = argv[2];

    ORB_SLAM3::System SLAM(vocab, camSettings, ORB_SLAM3::System::MONOCULAR, true);

    cv::VideoCapture cap("http://10.35.20.50:8080/video");
    if (!cap.isOpened()) {
        std::cerr << "Cannot open video stream." << std::endl;
        return -1;
    }

    cv::Mat frame;
    double tframe;
    while (true) {
        tframe = (double)cv::getTickCount();

        if (!cap.read(frame)) continue;

        SLAM.TrackMonocular(frame, tframe / cv::getTickFrequency());

        cv::imshow("Live", frame);
        if (cv::waitKey(1) == 27) break; // ESC to exit
    }

    SLAM.Shutdown();
    return 0;
}
