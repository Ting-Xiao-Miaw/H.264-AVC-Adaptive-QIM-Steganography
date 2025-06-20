import os
import shutil

def log_print(msg, log_file):
    print(msg)
    with open(log_file, "a", encoding="utf-8") as f:
        f.write(msg + "\n")


def clean_logs(keep_exts=[".py"]):
    os.makedirs("log", exist_ok=True)
    for fname in os.listdir("."):
        if os.path.isfile(fname) and not any(fname.endswith(ext) for ext in keep_exts):
            try:
                shutil.move(fname, os.path.join("log", fname))
            except Exception as e:
                print(f"移動 {fname} 到 log 資料夾失敗")
