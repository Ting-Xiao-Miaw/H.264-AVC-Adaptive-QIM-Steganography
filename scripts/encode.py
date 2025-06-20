import os
import shutil
import subprocess
from logger import log_print, clean_logs

VIDEOS_DIR = "../videos"
RESULTS_DIR = "../results"
EXE_PATH = "../exes/lencod.exe"
LOG_FILE = "encode_log.txt"

METHOD_QP = {
    "Ma": [23, 28, 33],
    "Lin": [23, 28, 33],
    "Proposed": [23, 28, 33],
}


def run_encode(video_name, qp):
    # Run the lencod.exe for a given video and QP
    
    valid_methods = [m for m, qps in METHOD_QP.items() if qp in qps]
    
    if not valid_methods:
        log_print(f"[Skipped] QP={qp} not supported by any method", LOG_FILE)
        return False
    
    sample_method = valid_methods[0]
    cfg_path = os.path.join(RESULTS_DIR, video_name, f"qp{qp}", sample_method, "encoder.cfg")
    if not os.path.exists(cfg_path):
        log_print(f"[Skipped] Missing encoder.cfg: {cfg_path}", LOG_FILE)
        return False

    log_print(f"[Encoding] {video_name}, QP={qp}", LOG_FILE)
    result = subprocess.run([EXE_PATH, "-d", cfg_path], capture_output=True, text=True)

    if result.returncode != 0:
        log_print(f"[Error] Encoding failed: {video_name}, QP={qp}", LOG_FILE)
        log_print(result.stderr, LOG_FILE)
        return False

    log_print(f"[Success] {video_name} QP={qp}", LOG_FILE)

    # Raw output filenames
    raw_264 = f"{video_name}.264"
    raw_yuv = f"{video_name}_rec.yuv"

    # Copy outputs to method folders
    for method, qps in METHOD_QP.items():
        if qp not in qps:
            continue
        
        target_dir = os.path.join(RESULTS_DIR, video_name, f"qp{qp}", method)

        # Move and rename
        try:
            shutil.copy(raw_264, os.path.join(target_dir, "original.264"))
            shutil.copy(raw_yuv, os.path.join(target_dir, "original_rec.yuv"))
            log_print(f"[Copied] Outputs saved to {target_dir}", LOG_FILE)
        except FileNotFoundError as e:
            log_print(f"[Error] Failed to copy outputs to {target_dir} : {e}", LOG_FILE)
            return False

    # Clean up temporary files
    os.remove(raw_264)
    os.remove(raw_yuv)
    return True


def run_all_encodes():
    # Run encoding for all videos and QPs 
    success = []
    fail = []

    yuv_files = [f for f in os.listdir(VIDEOS_DIR) if f.lower().endswith(".yuv")]
    video_names = [os.path.splitext(f)[0] for f in yuv_files]
    all_qps = sorted(set(qp for qps in METHOD_QP.values() for qp in qps))

    for video in video_names:
        for qp in all_qps:
            ok = run_encode(video, qp)
            if ok:
                success.append((video, qp))
            else:
                fail.append((video, qp))          

    log_print("\n====== Encoding Summary ======", LOG_FILE)
    log_print(f"Success count: {len(success)}", LOG_FILE)
    log_print(f"Failure count: {len(fail)}", LOG_FILE)
    if fail:
        log_print("Failure items:", LOG_FILE)
        for v, qp in fail:
            log_print(f" - {v} (QP={qp})", LOG_FILE)
            
    clean_logs()
    
    
if __name__ == "__main__":
    run_all_encodes()