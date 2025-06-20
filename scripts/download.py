import os

BASE_FOLDER = "../raw"

def download_videos(base_folder=BASE_FOLDER):
    # Download a set of CIF videos from Xiph.org

    VIDEO_URLS = {
        "akiyo": "https://media.xiph.org/video/derf/y4m/akiyo_cif.y4m",
        "bowing": "https://media.xiph.org/video/derf/y4m/bowing_cif.y4m",
        "bridge_close": "https://media.xiph.org/video/derf/y4m/bridge_close_cif.y4m",
        "bridge_far": "https://media.xiph.org/video/derf/y4m/bridge_far_cif.y4m",
        "bus": "https://media.xiph.org/video/derf/y4m/bus_cif.y4m",
        "city": "https://media.xiph.org/video/derf/y4m/city_cif.y4m",
        "coastguard": "https://media.xiph.org/video/derf/y4m/coastguard_cif.y4m",
        "container": "https://media.xiph.org/video/derf/y4m/container_cif.y4m",
        "crew": "https://media.xiph.org/video/derf/y4m/crew_cif.y4m",
        "deadline": "https://media.xiph.org/video/derf/y4m/deadline_cif.y4m",
        "flower": "https://media.xiph.org/video/derf/y4m/flower_cif.y4m",
        "foreman": "https://media.xiph.org/video/derf/y4m/foreman_cif.y4m",
        "hall_monitor": "https://media.xiph.org/video/derf/y4m/hall_monitor_cif.y4m",
        "harbour": "https://media.xiph.org/video/derf/y4m/harbour_cif.y4m",
        "highway": "https://media.xiph.org/video/derf/y4m/highway_cif.y4m",
        "husky": "https://media.xiph.org/video/derf/y4m/husky_cif.y4m",
        "mad900": "https://media.xiph.org/video/derf/y4m/mad900_cif.y4m",
        "mobile": "https://media.xiph.org/video/derf/y4m/mobile_cif.y4m",
        "mother_daughter": "https://media.xiph.org/video/derf/y4m/mother_daughter_cif.y4m",
        "news": "https://media.xiph.org/video/derf/y4m/news_cif.y4m",
        "pamphlet": "https://media.xiph.org/video/derf/y4m/pamphlet_cif.y4m",
        "paris": "https://media.xiph.org/video/derf/y4m/paris_cif.y4m",
        "sign_irene": "https://media.xiph.org/video/derf/y4m/sign_irene_cif.y4m",
        "silent": "https://media.xiph.org/video/derf/y4m/silent_cif.y4m",
        "soccer": "https://media.xiph.org/video/derf/y4m/soccer_cif.y4m",
        "students": "https://media.xiph.org/video/derf/y4m/students_cif.y4m",
        "tempete": "https://media.xiph.org/video/derf/y4m/tempete_cif.y4m",
        "waterfall": "https://media.xiph.org/video/derf/y4m/waterfall_cif.y4m",
    }

    # Create the output directory
    os.makedirs(base_folder, exist_ok=True)

    # Download each video if it does not already exist
    for video_name, url in VIDEO_URLS.items():
        video_path = os.path.join(base_folder, f"{video_name}.y4m")

        if os.path.exists(video_path):
            print(f"[Exist] {video_name}.y4m, skip download")
        else:
            print(f"[Downloading] {video_name}.y4m")
            os.system(f"curl -o {video_path} {url}")


if __name__ == "__main__":
    download_videos()