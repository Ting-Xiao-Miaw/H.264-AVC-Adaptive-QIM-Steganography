import os
import subprocess

RAW_FOLDER = "../raw"
VIDEO_FOLDER = "../videos"

def convert_y4m_to_yuv(raw_folder=RAW_FOLDER, video_folder=VIDEO_FOLDER):
    # Converts all .y4m video files to .yuv format using ffmpeg
    
    os.makedirs(video_folder, exist_ok=True)

    for filename in os.listdir(raw_folder):
        if filename.endswith(".y4m"):
            input_path = os.path.join(raw_folder, filename)
            output_filename = os.path.splitext(filename)[0] + ".yuv"
            output_path = os.path.join(video_folder, output_filename)

            # ffmpeg command to convert y4m to yuv
            cmd = [
                "ffmpeg",
                "-y",   # Overwrite output file if it exists
                "-i", input_path,
                "-pix_fmt", "yuv420p",  # Use standard 4:2:0 pixel format
                output_path
            ]

            print(f"[Converting] {filename} -> {output_filename}")
            subprocess.run(cmd, check=True)

    print("[Done] All y4m files converted to yuv format.")
  
    
if __name__ == "__main__":
    convert_y4m_to_yuv()