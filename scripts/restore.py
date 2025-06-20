import os
import shutil
import subprocess
from logger import log_print, clean_logs

RESULTS_DIR = "../results"
EXE_PATH = "../exes/ldecod.exe"
LOG_FILE = "restore_log.txt"

METHOD_QP = {
    "Ma": [23, 28, 33],
    "Lin": [23, 28, 33],
    "Proposed": [23, 28, 33],
}


def run_extract(video_name, qp, method):
    # Run the ldecod.exe -mode for a given video and QP
    
    cfg_path = os.path.join(RESULTS_DIR, video_name, f"qp{qp}", method, "decoder_lrestore.cfg")
    if not os.path.exists(cfg_path):
        log_print(f"[Skipped] Missing decoder.cfg: {cfg_path}", LOG_FILE)
        return False

    log_print(f"[Restoring]: {video_name}, QP={qp}, Method={method}", LOG_FILE)
    
    # Determine -mode method
    mode_flag = {
        "Ma": "M",
        "Lin": "L",
        "Proposed": "P",
    }.get(method)

    if mode_flag is None:
        log_print(f"[Error] Unknown method: {method}", LOG_FILE)
        return False

    # Run ldecod.exe with the configuration and mode
    result = subprocess.run([EXE_PATH, "-d", cfg_path, "-mode", mode_flag], capture_output=True, text=True)

    if result.returncode != 0:
        log_print(f"[Failed] {video_name} QP={qp} Method={method}", LOG_FILE)
        log_print(result.stderr, LOG_FILE)
        return False

    log_print(f"[Success] {video_name} QP={qp} Method={method}", LOG_FILE)

    # Move the receiver_message to results folder
    target_dir = os.path.join(RESULTS_DIR, video_name, f"qp{qp}", method)
    
    try:
        shutil.move("receiver_message.txt", os.path.join(target_dir, "receiver_message.txt"))
        log_print(f"[Saved] receiver_message.txt moved to {target_dir}", LOG_FILE)
        return True
    except FileNotFoundError as e:
        log_print(f"[Error] receiver_message.txt missing: {e}", LOG_FILE)
        return False

def run_all_restores():
    # Run restoring for all videos and QPs
    success = []
    fail = []

    video_names = [f for f in os.listdir(RESULTS_DIR) if os.path.isdir(os.path.join(RESULTS_DIR, f))]

    for video in video_names:
        for method, qps in METHOD_QP.items():
            for qp in qps:
                ok = run_extract(video, qp, method)
                if ok:
                    success.append((video, qp, method))
                else:
                    fail.append((video, qp, method))

    log_print("\n==== Restoring Summary ====", LOG_FILE)
    log_print(f"Success count: {len(success)}", LOG_FILE)
    log_print(f"Failure count: {len(fail)}", LOG_FILE)
    if fail:
        log_print("Failed items:", LOG_FILE)
        for v, q, m in fail:
            log_print(f" - {v}, QP={q}, Method={m}", LOG_FILE)

    clean_logs()

if __name__ == "__main__":
    run_all_restores()