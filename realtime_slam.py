# realtime_slam.py
import cv2
import subprocess
import os
import time
import threading
import queue

class RealTimeSLAM:
    def __init__(self, vocab_path, settings_path):
        self.vocab_path = vocab_path
        self.settings_path = settings_path
        self.frame_queue = queue.Queue(maxsize=100)
        self.running = True
        
    def capture_frames(self, ip_url):
        """Capture frames from IP camera"""
        cap = cv2.VideoCapture(ip_url)
        frame_id = 0
        
        while self.running:
            ret, frame = cap.read()
            if not ret:
                print("Failed to grab frame")
                time.sleep(0.1)
                continue
            
            # Save frame temporarily
            filename = f"temp_frame_{frame_id % 100:04d}.png"
            cv2.imwrite(filename, frame)
            
            # Add to queue with timestamp
            timestamp = time.time()
            self.frame_queue.put((filename, timestamp))
            
            # Show preview
            cv2.imshow("Phone Camera", frame)
            
            frame_id += 1
            
            # Check for ESC key
            if cv2.waitKey(1) & 0xFF == 27:
                self.running = False
                break
        
        cap.release()
        cv2.destroyAllWindows()
    
    def process_frames(self):
        """Process frames with ORB-SLAM3"""
        while self.running or not self.frame_queue.empty():
            try:
                # Get batch of frames
                frames_batch = []
                timestamps = []
                
                # Get up to 50 frames
                for _ in range(50):
                    filename, timestamp = self.frame_queue.get(timeout=1)
                    frames_batch.append(filename)
                    timestamps.append(timestamp)
                
                # Create rgb.txt for this batch
                with open("temp_rgb.txt", "w") as f:
                    f.write("# color images\n")
                    f.write("# timestamp filename\n")
                    for i, (filename, timestamp) in enumerate(zip(frames_batch, timestamps)):
                        f.write(f"{timestamp:.6f} {filename}\n")
                
                # Run ORB-SLAM3 on this batch
                print(f"Processing batch of {len(frames_batch)} frames...")
                cmd = [
                    "./Examples/Monocular/mono_tum",
                    self.vocab_path,
                    self.settings_path,
                    "."
                ]
                subprocess.run(cmd, capture_output=True)
                
                # Clean up processed frames
                for filename in frames_batch:
                    if os.path.exists(filename):
                        os.remove(filename)
                        
            except queue.Empty:
                continue
    
    def run(self, ip_url):
        """Main run method"""
        print("Starting Real-Time SLAM with IP Camera...")
        print("Press ESC to stop")
        
        # Start threads
        capture_thread = threading.Thread(target=self.capture_frames, args=(ip_url,))
        process_thread = threading.Thread(target=self.process_frames)
        
        capture_thread.start()
        process_thread.start()
        
        capture_thread.join()
        process_thread.join()
        
        print("SLAM stopped")

# Usage
if __name__ == "__main__":
    slam = RealTimeSLAM(
        vocab_path="Vocabulary/ORBvoc.txt",
        settings_path="Examples/Monocular/TUM1.yaml"
    )
    slam.run("http://10.35.20.50:8080/video")