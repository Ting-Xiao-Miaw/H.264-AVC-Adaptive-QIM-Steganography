import os

RESULTS_DIR = "../results"

METHOD_QP = {
    "Ma": [23, 28, 33],
    "Lin": [23, 28, 33],
    "Proposed": [23, 28, 33],
}


def read_bits(filepath):
    try:
        with open(filepath, 'r') as f:
            content = f.read().strip()
            return [c for c in content if c in '01']
    except FileNotFoundError:
        return None


def compare_files(sender_path, receiver_path, output_path):
    # Compare sender and receiver bitstreams
    bits1 = read_bits(sender_path)
    bits2 = read_bits(receiver_path)

    method = os.path.basename(os.path.dirname(sender_path))
    qp = os.path.basename(os.path.dirname(os.path.dirname(sender_path)))[2:]
    video = os.path.basename(os.path.dirname(os.path.dirname(os.path.dirname(sender_path))))
    tag = f"[{video}][QP={qp}][{method}]"

    lines = [tag]
    summary = ""

    if bits1 is None:
        lines.append(f"[Error] sender_message.txt not found: {sender_path}")
        summary = f"{tag}: sender_file missing"
    elif bits2 is None:
        lines.append(f"[Error] receiver_message.txt not found: {receiver_path}")
        summary = f"{tag}: receiver_file missing"
    else:
        len1, len2 = len(bits1), len(bits2)
        min_len = min(len1, len2)
        errors = [(i, bits1[i], bits2[i]) for i in range(min_len) if bits1[i] != bits2[i]]

        lines.extend([
            f"Sender message length  : {len1}",
            f"Receiver message length: {len2}",
            f"Compared first {min_len} bits"
        ])

        if len1 != len2:
            lines.append("Warning: Message lengths are different.")

        if not errors:
            lines.append("Bitstreams are identical.")
            summary = f"{tag}: Identical"
        else:
            lines.append(f"Found {len(errors)} mismatched bits:")
            for i, b1, b2 in errors[:10]:
                lines.append(f"  At position {i}: expected {b1}, got {b2}")
            if len(errors) > 10:
                lines.append(f"  ... and {len(errors) - 10} more errors.")
            accuracy = (min_len - len(errors)) / min_len * 100
            lines.append(f"Accuracy: {accuracy:.2f}%")
            summary = f"{tag}: {accuracy:.2f}%"

    # Write to compare.txt in the target directory
    with open(output_path, "w") as f:
        f.write("\n".join(lines) + "\n")

    return summary

def compare_all():
    # Compare sender and receiver messages for all videos, methods, and QPs.
    
    video_names = [f for f in os.listdir(RESULTS_DIR) if os.path.isdir(os.path.join(RESULTS_DIR, f))]
    summaries = []

    for video in video_names:
        for method, qps in METHOD_QP.items():
            for qp in qps:
                base_path = os.path.join(RESULTS_DIR, video, f"qp{qp}", method)
                sender = os.path.join(base_path, "sender_message.txt")
                receiver = os.path.join(base_path, "receiver_message.txt")
                output_txt = os.path.join(base_path, "compare.txt")

                summary = compare_files(sender, receiver, output_txt)
                summaries.append(summary)

    # Print summary results to console
    print("\n==== Comparison Summary ====")
    for line in summaries:
        print(line)


if __name__ == "__main__":
    compare_all()