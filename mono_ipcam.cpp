// mono_ipcam.cpp - REAL-TIME VERSION
#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <System.h>

using namespace std;

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        cerr << endl << "Usage: ./mono_ipcam path_to_vocabulary path_to_settings ip_camera_url" << endl;
        return 1;
    }

    string voc_path = argv[1];
    string settings_path = argv[2];
    string ip_url = argv[3];

    // Create SLAM system
    ORB_SLAM3::System SLAM(voc_path, settings_path, ORB_SLAM3::System::MONOCULAR, true);
    
    // Open IP camera
        // Open IP camera instead of loading images from file
    cv::VideoCapture cap(string(argv[3]));
    if(!cap.isOpened())
    {
        cerr << endl << "Failed to open IP camera: " << argv[3] << endl;
        return 1;
    }

    cout << endl << "-------" << endl;
    cout << "Start processing IP camera stream..." << endl;
    cout << "Press ESC to stop" << endl;

    // Main loop
    cv::Mat im;
    auto start_time = chrono::steady_clock::now();
    int ni = 0;
    
    while(true)
    {
        // Read image from IP camera
        cap >> im;
        if(im.empty())
        {
            cerr << endl << "Failed to grab image" << endl;
            break;
        }

        double tframe = chrono::duration<double>(chrono::steady_clock::now() - start_time).count();

        // Pass the image to the SLAM system
        SLAM.TrackMonocular(im, tframe);

        // Check for ESC key
        char c = (char)cv::waitKey(30);
        if(c == 27)
            break;
            
        ni++;
    }

    // Stop all threads
    SLAM.Shutdown();

    // Save camera trajectory
    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory_IPCam.txt");

    cout << "Trajectory saved to KeyFrameTrajectory_IPCam.txt" << endl;
    cout << "Processed " << frame_id << " frames total" << endl;

    return 0;
}