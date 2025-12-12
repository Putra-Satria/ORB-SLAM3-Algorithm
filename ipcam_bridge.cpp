// ipcam_bridge.cpp - FIXED VERSION
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <iomanip>
#include <sstream>

int main() {
    std::string ip_url = "http://10.35.20.50:8080/video";
    
    cv::VideoCapture cap(ip_url);
    if (!cap.isOpened()) {
        std::cout << "Cannot open IP camera: " << ip_url << std::endl;
        return -1;
    }
    
    std::cout << "IP Camera opened successfully!" << std::endl;
    
    // Create directory for frames
    system("mkdir -p ipcam_frames");
    
    // Create timestamp file (like TUM dataset format)
    std::ofstream timestamp_file("rgb.txt");
    timestamp_file << "# color images\n";
    timestamp_file << "# file: 'ipcam_frames'\n";
    timestamp_file << "# timestamp filename\n";
    
    int frame_count = 0;
    cv::Mat frame;
    auto start_time = std::chrono::steady_clock::now();
    
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cout << "Empty frame received, retrying..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // Calculate timestamp in seconds
        auto current_time = std::chrono::steady_clock::now();
        double timestamp = std::chrono::duration<double>(current_time - start_time).count();
        
        // Pad frame count with zeros (6 digits)
        std::stringstream ss;
        ss << std::setw(6) << std::setfill('0') << frame_count;
        std::string frame_num = ss.str();
        
        // Save frame
        std::string filename = "ipcam_frames/" + frame_num + ".png";
        cv::imwrite(filename, frame);
        
        // Write to timestamp file
        timestamp_file << std::fixed << std::setprecision(6) << timestamp << " " << filename << "\n";
        
        // Show preview
        cv::imshow("Phone Camera", frame);
        
        std::cout << "Frame " << frame_count << " saved at " << timestamp << "s" << std::endl;
        
        frame_count++;
        
        // Break on ESC key or after 1000 frames
        char key = cv::waitKey(30);
        if (key == 27 || frame_count >= 1000) {  // ESC key
            break;
        }
    }
    
    cap.release();
    cv::destroyAllWindows();
    timestamp_file.close();
    
    std::cout << "\nSaved " << frame_count << " frames to ipcam_frames/" << std::endl;
    std::cout << "You can now run ORB-SLAM3 on these frames!" << std::endl;
    
    return 0;
}