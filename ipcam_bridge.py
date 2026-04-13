# ipcam_bridge.py
import cv2
import time
import os

# Create directory
os.makedirs("ipcam_frames", exist_ok=True)

# Open IP camera
cap = cv2.VideoCapture("http://10.35.20.50:8080/video")

if not cap.isOpened():
    print("Cannot open IP camera")
    exit()

print("IP Camera opened successfully!")

# Create timestamp file
timestamp_file = open("rgb.txt", "w")
timestamp_file.write("# color images\n")
timestamp_file.write("# file: 'ipcam_frames'\n")
timestamp_file.write("# timestamp filename\n")

frame_count = 0
start_time = time.time()

while True:
    ret, frame = cap.read()
    if not ret:
        print("Empty frame received, retrying...")
        time.sleep(0.1)
        continue
    
    # Calculate timestamp
    timestamp = time.time() - start_time
    
    # Save frame
    filename = f"ipcam_frames/{frame_count:06d}.png"
    cv2.imwrite(filename, frame)
    
    # Write to timestamp file
    timestamp_file.write(f"{timestamp:.6f} {filename}\n")
    
    # Show preview
    cv2.imshow("Phone Camera", frame)
    
    print(f"Frame {frame_count} saved at {timestamp:.2f}s")
    
    frame_count += 1
    
    # Break on ESC key or after 500 frames
    key = cv2.waitKey(30) & 0xFF
    if key == 27 or frame_count >= 500:  # ESC key
        break

cap.release()
cv2.destroyAllWindows()
timestamp_file.close()

print(f"\nSaved {frame_count} frames to ipcam_frames/")
print("You can now run ORB-SLAM3 on these frames!")