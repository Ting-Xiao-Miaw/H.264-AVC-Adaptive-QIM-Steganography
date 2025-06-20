import os

RESULTS_DIR = "../results"

METHOD_QP = {
    "Ma": [23, 28, 33],
    "Lin": [23, 28, 33],
    "Proposed": [23, 28, 33],
}


def get_total_frames(encoder_cfg_path):
    # Extract the number of frames to be encoded from encoder.cfg
    if not os.path.exists(encoder_cfg_path):
        return None
    with open(encoder_cfg_path, 'r') as f:
        for line in f:
            if "FramesToBeEncoded" in line:
                parts = line.split("=")
                if len(parts) == 2:
                    return int(parts[1].strip().split("#")[0])
    return None


def calculate_bitrate(file_path, total_frames):
    # Calculate bitrate (kbps) for the given file and frame count
    if not os.path.exists(file_path) or total_frames is None or total_frames == 0:
        return None
    file_size_bytes = os.path.getsize(file_path)
    total_bits = file_size_bytes * 8
    return (total_bits / total_frames) / 1000  # kbps


def bitrate_comparison():
    # Compare bitrates between original and embedded videos
    video_names = [v for v in os.listdir(RESULTS_DIR) if os.path.isdir(os.path.join(RESULTS_DIR, v))]

    for video in sorted(video_names):
        for method, qps in METHOD_QP.items():
            for qp in qps:
                base_path = os.path.join(RESULTS_DIR, video, f"qp{qp}", method)
                orig_path = os.path.join(base_path, "original.264")
                emb_path = os.path.join(base_path, "embedded.264")
                cfg_path = os.path.join(base_path, "encoder.cfg")
                bri_path = os.path.join(base_path, "bri.txt")

                total_frames = get_total_frames(cfg_path)
                bitrate_orig = calculate_bitrate(orig_path, total_frames)
                bitrate_emb = calculate_bitrate(emb_path, total_frames)

                if bitrate_orig is None or bitrate_emb is None:
                    continue
                    
                bri = ((bitrate_emb - bitrate_orig) / bitrate_orig * 100) if bitrate_orig > 0 else 0
                header = f"{'Video':<15} {'Method':<10} {'QP':<5} {'Orig(kbps)':<12} {'Emb(kbps)':<12} {'BRI(%)':<8}"
                result = f"{video:<15} {method:<10} {qp:<5} {bitrate_orig:<12.2f} {bitrate_emb:<12.2f} {bri:<8.2f}"
                print(header)
                print("-" * 75)
                print(result)
                
                # Write to bri.txt
                with open(bri_path, "w") as f:
                    f.write(header + "\n")
                    f.write("-" * 75 + "\n")
                    f.write(result + "\n")


if __name__ == "__main__":
    bitrate_comparison()
