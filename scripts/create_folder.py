import os

VIDEOS_DIR = "../videos"
RESULTS_DIR = "../results"

METHOD_QP = {
    "Ma": [23, 28, 33],
    "Lin": [23, 28, 33],
    "Proposed": [23, 28, 33],
}

# Retrieves a list of base filenames of YUV videos
def get_video_names():
    return [
        os.path.splitext(f)[0]
        for f in os.listdir(VIDEOS_DIR)
        if f.lower().endswith('.yuv')
    ]

# Creates a directory structure to store experimental results
# Example: ../results/akiyo/qp28/Proposed/
def create_structure():
    video_names = get_video_names()
    for video in video_names:
        for method, qps in METHOD_QP.items():
            for qp in qps:
                path = os.path.join(RESULTS_DIR, video, f"qp{qp}", method)
                os.makedirs(path, exist_ok=True)
    print("Folder structure created successfully.")


if __name__ == "__main__":
    create_structure()