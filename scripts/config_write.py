import os
import re
from create_folder import get_video_names

VIDEOS_DIR = "../videos"
ENCODER_CFG_DIR = "../cfg/encoder_base_cfg"
DECODER_CFG_DIR = "../cfg/decoder_base_cfg"
RESULTS_DIR = "../results"

METHOD_QP = {
    "Ma": [23, 28, 33],
    "Lin": [23, 28, 33],
    "Proposed": [23, 28, 33],
}

# Regex patterns for QP
pattern_map = {
    "QPISlice": re.compile(r"^(QPISlice\s*=\s*)(\d+)(\s*#.*)?$"),
    "QPPSlice": re.compile(r"^(QPPSlice\s*=\s*)(\d+)(\s*#.*)?$"),
    "QPBSlice": re.compile(r"^(QPBSlice\s*=\s*)(\d+)(\s*#.*)?$"),
}

# Regex patterns for input, output, and reference file
inputfile_pattern = re.compile(r'^(InputFile\s*=\s*)"(.*)"(\s*#.*)?$')
outputfile_pattern = re.compile(r'^(OutputFile\s*=\s*)"(.*)"(\s*#.*)?$')
reffile_pattern = re.compile(r'^(RefFile\s*=\s*)"(.*)"(\s*#.*)?$')

# def get_video_names():
#     return [
#         os.path.splitext(f)[0]
#         for f in os.listdir(VIDEOS_DIR)
#         if f.lower().endswith('.yuv')
#     ]

def generate_encoder_cfgs(video_name, qp, method):
    # Generates a method-specific encoder.cfg based on a base cfg
    base_cfg_path = os.path.join(ENCODER_CFG_DIR, f"{video_name}.cfg")
    if not os.path.exists(base_cfg_path):
        print(f"[Skip] Cannot find base encoder cfg: {base_cfg_path}")
        return

    with open(base_cfg_path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    updated_lines = []
    for line in lines:
        # Update input line
        input_match = inputfile_pattern.match(line)
        if input_match:
            prefix, _, comment = input_match.groups()
            comment = comment or ""
            new_path = os.path.join("..", "videos", f"{video_name}.yuv").replace("\\", "/")
            new_line = f'{prefix}"{new_path}"{comment}\n'
            updated_lines.append(new_line)
            continue

        # Update QP line
        for _, pattern in pattern_map.items():
            match = pattern.match(line)
            if match:
                prefix, _, comment = match.groups()
                comment = comment or ""
                new_line = f"{prefix}{qp}{comment}\n"
                updated_lines.append(new_line)
                break
        else:
            updated_lines.append(line)

    # Create target directory and write updated config
    target_dir = os.path.join(RESULTS_DIR, video_name, f"qp{qp}", method)
    os.makedirs(target_dir, exist_ok=True)
    target_cfg_path = os.path.join(target_dir, "encoder.cfg")

    with open(target_cfg_path, "w", encoding="utf-8") as f:
        f.writelines(updated_lines)
    print(f"[Generated] {target_cfg_path}")


def generate_decoder_cfgs(video_name, qp, method):
    base_cfg_path = os.path.join(DECODER_CFG_DIR, f"{video_name}.cfg")
    if not os.path.exists(base_cfg_path):
        print(f"[Skip] Missing base decoder cfg: {base_cfg_path}")
        return

    with open(base_cfg_path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    def modify_lines(version):
        updated_lines = []
        for line in lines:
            if inputfile_pattern.match(line):
                prefix, _, comment = inputfile_pattern.match(line).groups()
                comment = comment or ""
                filename = "original.264" if version == "ldecod" else "embedded.264"
                filepath = os.path.join("..", "results", video_name, f"qp{qp}", method, filename).replace("\\", "/")
                updated_lines.append(f'{prefix}"{filepath}"{comment}\n')
            elif outputfile_pattern.match(line):
                prefix, _, comment = outputfile_pattern.match(line).groups()
                comment = comment or ""
                filename = "original_dec.yuv" if version == "ldecod" else "embedded_dec.yuv"
                filepath = os.path.join("..", "results", video_name, f"qp{qp}", method, filename).replace("\\", "/")
                updated_lines.append(f'{prefix}"{filepath}"{comment}\n')
            elif reffile_pattern.match(line):
                prefix, _, comment = reffile_pattern.match(line).groups()
                comment = comment or ""
                filename = "original_rec.yuv" if version == "ldecod" else "embedded_rec.yuv"
                filepath = os.path.join("..", "results", video_name, f"qp{qp}", method, filename).replace("\\", "/")
                updated_lines.append(f'{prefix}"{filepath}"{comment}\n')
            else:
                updated_lines.append(line)
        return updated_lines

    target_dir = os.path.join(RESULTS_DIR, video_name, f"qp{qp}", method)
    os.makedirs(target_dir, exist_ok=True)

    # Generate ldecod and lrestore for original.264 and embedding.264
    for version in ["ldecod", "lrestore"]:
        cfg_lines = modify_lines(version)
        cfg_path = os.path.join(target_dir, f"decoder_{version}.cfg")
        with open(cfg_path, "w", encoding="utf-8") as f:
            f.writelines(cfg_lines)
        print(f"[Generated] {cfg_path}")


def generate_all_cfgs():
    # Generates all encoder and decoder config files
    video_names = get_video_names()
    for video in video_names:
        for method, qps in METHOD_QP.items():
            for qp in qps:
                generate_encoder_cfgs(video, qp, method)
                generate_decoder_cfgs(video, qp, method)


if __name__ == "__main__":
    generate_all_cfgs()