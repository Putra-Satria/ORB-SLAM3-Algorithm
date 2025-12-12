#!/usr/bin/env python3
import cv2
import os
import sys

def extract_frames(video_path, output_dir="extracted_frames", target_size=(640, 368)):
    """
    Extract frames from Tello MP4 video and resize appropriately
    """
    # Create output directory
    os.makedirs(output_dir, exist_ok=True)
    
    # Open video file
    cap = cv2.VideoCapture(video_path)
    
    if not cap.isOpened():
        print(f"Error: Could not open video file {video_path}")
        return False
    
    # Get video properties
    fps = cap.get(cv2.CAP_PROP_FPS)
    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    duration = total_frames / fps
    
    print(f"Video Info:")
    print(f"  Original resolution: 960x544")
    print(f"  Target resolution: {target_size[0]}x{target_size[1]}")
    print(f"  FPS: {fps}")
    print(f"  Total Frames: {total_frames}")
    print(f"  Duration: {duration:.2f} seconds")
    
    frame_count = 0
    saved_count = 0
    
    while True:
        ret, frame = cap.read()
        
        if not ret:
            break
        
        # Resize frame to target size (maintaining aspect ratio as close as possible)
        # 960x544 -> 640x368 maintains the same aspect ratio (~1.76)
        frame_resized = cv2.resize(frame, target_size)
        
        # Save frame
        filename = os.path.join(output_dir, f"{saved_count:06d}.png")
        cv2.imwrite(filename, frame_resized)
        saved_count += 1
        
        frame_count += 1
        
        # Progress indicator
        if frame_count % 50 == 0:
            print(f"Processed {frame_count}/{total_frames} frames...")
    
    cap.release()
    
    print(f"Extraction complete!")
    print(f"Saved {saved_count} frames to {output_dir}")
    
    return saved_count, fps

def create_timestamps(output_dir, num_frames, fps):
    """Create timestamps file for ORB-SLAM3"""
    timestamps_path = os.path.join(output_dir, "timestamps.txt")
    
    with open(timestamps_path, "w") as f:
        for i in range(num_frames):
            timestamp = i / fps
            f.write(f"{timestamp:.6f}\n")
    
    print(f"Created timestamps file: {timestamps_path}")
    return timestamps_path

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 extract_tello_frames.py <video_file.mp4>")
        print("Example: python3 extract_tello_frames.py TelloDrone.mp4")
        sys.exit(1)
    
    video_file = sys.argv[1]
    
    print(f"Processing: {video_file}")
    
    # Extract frames (640x368 maintains the 960x544 aspect ratio)
    num_frames, fps = extract_frames(video_file, "extracted_frames", (640, 368))
    create_timestamps("extracted_frames", num_frames, fps)
    
    print(f"\nReady for ORB-SLAM3! Run:")
    print(f"cd ~/ORB_SLAM3")
    print(f"./Examples/Monocular/mono_tum Vocabulary/ORBvoc.txt Examples/Monocular/Tello.yaml tello_video_processing/extracted_frames")
