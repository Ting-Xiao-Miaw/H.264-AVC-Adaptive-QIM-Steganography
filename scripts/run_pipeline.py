from download import download_videos
from y4m2yuv import convert_y4m_to_yuv
from create_folder import create_structure
from config_write import generate_all_cfgs
from encode import run_all_encodes
from decode import run_all_decodes
from embed import run_all_embeds
from restore import run_all_restores
from compare import compare_all
from psnr_ssim import compute_all
from bri import bitrate_comparison


def run_pipeline():
    download_videos()
    convert_y4m_to_yuv()
    create_structure()
    generate_all_cfgs()
    run_all_encodes()
    run_all_decodes()
    run_all_embeds()
    run_all_restores()
    compare_all()
    compute_all()
    bitrate_comparison()
   
 
if __name__ == "__main__":
    run_pipeline()