# H.264 Information Hiding Toolkit

This repository provides a complete automation framework for embedding, extracting, and analyzing information hiding techniques in H.264 compressed video streams. It supports batch processing across multiple videos, QP values, and embedding methods, and includes scripts for encoding, decoding, metric evaluation, and result logging.

## 📁 Project Structure

The following Python scripts compose the pipeline:

| Script | Description |
|--------|-------------|
| `download.py` | Downloads Y4M video files from [17] and stores them in `raw/`. |
| `y4m2yuv.py` | Converts Y4M files to YUV format using `ffmpeg`, and saves them to `videos/`. |
| `create_folder.py` | Automatically creates folder structures under `results/{video}/qp{qp}/{method}` to organize input/output files. |
| `config_write.py` | Generates `encoder.cfg` and `decoder.cfg` files for each experimental condition by modifying templates from `cfg/`. Edits include `QPISlice`, `InputFile`, and `OutputFile` fields. |
| `encode.py` | Encodes YUV files into H.264 format using `lencod.exe`, generating `original.264`. |
| `decode.py` | Decodes `original.264` back to YUV using `ldecod.exe`, and outputs `mode.txt` and `mapping.txt`. |
| `embed.py` | Performs information embedding via `lencod.exe` with the `-mode` parameter. Outputs `embedded.264` and `sender_message.txt`. |
| `restore.py` | Recovers embedded messages using `ldecod.exe` with the `-mode` parameter, producing `receiver_message.txt`. |
| `compare.py` | Compares `sender_message.txt` and `receiver_message.txt` under each result folder, computes bit error rate (BER), and records it in `compare.txt`. |
| `psnr_ssim.py` | Uses `ffmpeg` to decode both `original.264` and `embedded.264` into frames, calculates average PSNR and SSIM, and logs results to `psnr_ssim.txt`. |
| `bri.py` | Computes bit rate increase (BRI) between `original.264` and `embedded.264`, and writes to `bri.txt`. |
| `logger.py` | Handles debug messages and logs during script execution. |
| `run_pipeline.py` | Master script that runs the entire embedding pipeline across all videos, QPs, and methods, collecting and summarizing all results. |

## 🛠️ Requirements

- Python 3.x
- [ffmpeg](https://ffmpeg.org/)
