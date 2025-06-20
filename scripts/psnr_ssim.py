import os
import subprocess
import cv2
import numpy as np
from skimage.metrics import structural_similarity as ssim

RESULTS_DIR = "../results"
METHOD_QP = {
    "Ma": [23, 28, 33],
    "Lin": [23, 28, 33],
    "Proposed": [23, 28, 33],
}


def calculate_psnr(img1, img2):
    # Compute PSNR between two images  
    mse = np.mean((img1.astype(np.float32) - img2.astype(np.float32)) ** 2)
    if mse == 0:
        return float('inf')
    PIXEL_MAX = 255.0
    return 20 * np.log10(PIXEL_MAX / np.sqrt(mse))


def compare_frames(folder1, folder2):
    # Compare all frames in two folders and compute average PSNR and SSIM.
    files1 = sorted(os.listdir(folder1))
    files2 = sorted(os.listdir(folder2))

    psnr_list = []
    ssim_list = []

    for f1, f2 in zip(files1, files2):
        img1 = cv2.imread(os.path.join(folder1, f1))
        img2 = cv2.imread(os.path.join(folder2, f2))
        if img1 is None or img2 is None or img1.shape != img2.shape:
            continue

        psnr_value = calculate_psnr(img1, img2)
        ssim_value = ssim(img1, img2, channel_axis=-1)

        psnr_list.append(psnr_value)
        ssim_list.append(ssim_value)

    avg_psnr = np.mean(psnr_list) if psnr_list else 0
    avg_ssim = np.mean(ssim_list) if ssim_list else 0

    return avg_psnr, avg_ssim


def extract_frames(video_path, output_folder):
    # Extract frames from a video using ffmpeg
    os.makedirs(output_folder, exist_ok=True)
    subprocess.run([
        "ffmpeg", "-y", "-i", video_path,
        os.path.join(output_folder, "frame_%04d.png")
    ], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def compute_all():
    # Compute PSNR and SSIM between original and embedded videos for all videos
    video_names = [v for v in os.listdir(RESULTS_DIR) if os.path.isdir(os.path.join(RESULTS_DIR, v))]

    for video in video_names:
        for method, qps in METHOD_QP.items():
            for qp in qps:
                path = os.path.join(RESULTS_DIR, video, f"qp{qp}", method)
                original_path = os.path.join(path, "original.264")
                embedded_path = os.path.join(path, "embedded.264")
                tag = f"【{video}】【QP={qp}】【{method}】"

                if not os.path.exists(original_path) or not os.path.exists(embedded_path):
                    print(f"{tag}: Missing video files. Skipping.")
                    continue

                orig_frame_dir = os.path.join(path, "original_frames")
                emb_frame_dir = os.path.join(path, "embedded_frames")

                extract_frames(original_path, orig_frame_dir)
                extract_frames(embedded_path, emb_frame_dir)

                avg_psnr, avg_ssim = compare_frames(orig_frame_dir, emb_frame_dir)

                # Write result into psnr_ssim.txt
                with open(os.path.join(path, "psnr_ssim.txt"), "w") as f:
                    f.write(f"{tag}\n")
                    f.write(f"Average PSNR: {avg_psnr:.2f} dB\n")
                    f.write(f"Average SSIM: {avg_ssim:.4f}\n")

                print(f"{tag}: PSNR={avg_psnr:.2f} dB, SSIM={avg_ssim:.4f}")
                

if __name__ == "__main__":
    compute_all()