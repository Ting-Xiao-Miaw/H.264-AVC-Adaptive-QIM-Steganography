import os
import shutil
import subprocess
from logger import log_print, clean_logs

RESULTS_DIR = "../results"
EXE_PATH = "../exes/ldecod.exe"
LOG_FILE = "decode_log.txt"

METHOD_QP = {
    "Ma": [23, 28, 33],
    "Lin": [23, 28, 33],
    "Proposed": [23, 28, 33],
}


def run_decode(video_name, qp):
    # Run the ldecod.exe for a given video and QP
    
    valid_methods = [m for m, qps in METHOD_QP.items() if qp in qps]
    
    if not valid_methods:
        log_print(f"[Skipped] QP={qp} not supported by any method", LOG_FILE)
        return False
    
    sample_method = valid_methods[0]
    cfg_path = os.path.join(RESULTS_DIR, video_name, f"qp{qp}", sample_method, "decoder_ldecod.cfg")
    if not os.path.exists(cfg_path):
        log_print(f"[Skipped] Missing decoder.cfg: {cfg_path}", LOG_FILE)
        return False
    
    log_print(f"[Decoding] {video_name} QP={qp}", LOG_FILE)
    result = subprocess.run([EXE_PATH, "-d", cfg_path], capture_output=True, text=True)
    
    if result.returncode != 0:
        log_print(f"[Error] Decoding failed: {video_name} QP={qp}", LOG_FILE)
        log_print(result.stderr, LOG_FILE)
        return False
    
    
    log_print(f"[Success] {video_name} QP={qp}", LOG_FILE)
    
    # Move mapping.txt and mode.txt to each method folder
    for fname in ["mapping.txt", "mode.txt"]:
        if not os.path.exists(fname):
            log_print(f"[Warning] Missing {fname}", LOG_FILE)
            continue
        for method in valid_methods:
            target = os.path.join(RESULTS_DIR, video_name, f"qp{qp}", method, fname)
            try:
                shutil.copy(fname, target)
                log_print(f"[Copied] {fname} saved to {target}", LOG_FILE)
            except Exception as e:
                log_print(f"[Error] Failed to copy {fname}: {e}", LOG_FILE)
        os.remove(fname)  # Remove the original file
                
    return True
        

def run_all_decodes():
    # Run decoding for all videos and QPs
    success = []
    fail = []
    
    video_names = [f for f in os.listdir(RESULTS_DIR) if os.path.isdir(os.path.join(RESULTS_DIR, f))] 
    all_qps = sorted(set(qp for qps in METHOD_QP.values() for qp in qps))
    
    for video in video_names:
        for qp in all_qps:
            ok = run_decode(video, qp)
            if ok:
                success.append((video, qp))
            else:
                fail.append((video, qp))
    
    log_print("\n==== Decoding Summary ====", LOG_FILE)
    log_print(f"Success count: {len(success)}", LOG_FILE)
    log_print(f"Failure count: {len(fail)}", LOG_FILE)
    if fail:
        log_print("Failure items:", LOG_FILE)
        for v, q in fail:
            log_print(f" - {v}, QP={q}", LOG_FILE)

    clean_logs()


if __name__ == "__main__":
    run_all_decodes()