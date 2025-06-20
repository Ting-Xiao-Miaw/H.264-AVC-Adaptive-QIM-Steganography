# H.264-AVC-Adaptive-QIM-Steganography

This repository provides the official implementation of our adaptive information hiding scheme for the H.264/AVC video compression standard. The method is designed to achieve a balanced trade-off among **hiding capacity**, **visual quality**, and **compression efficiency**, with demonstrated robustness and scalability across various encoding conditions.

## 🧠 Overview

This study proposes a compressed-domain steganographic method that enhances both embedding performance and resilience by leveraging:

- **Matrix encoding with mod-4 QIM**
- **Hamming coding theory for syndrome embedding**
- **Adaptive coefficient selection in QDCT domain**

The original 4-bit message is first transformed into a **syndrome**, which is embedded into quantized DCT coefficients (`QDCT`) through an **optimal distortion-minimizing adjustment strategy**. This approach allows flexible embedding capacity based on block complexity and minimizes perceptual distortion via adaptive QIM.

## ✨ Key Features

- 🔢 **High Hiding Capacity**: Achieves an average **78.97% improvement** over existing methods.
- 👁️‍🗨️ **High Visual Fidelity under Heavy Embedding**: Maintains good PSNR and SSIM even under high embedding rates.
- 📦 **Compression-Aware Design**: Embedding is performed directly in the quantized transform domain (QDCT), preserving bitrate efficiency.
- 📈 **Scalable and Robust**: Performance remains linear and stable across different QPs, indicating strong adaptability to encoding parameters.

### 🖼️ Visual Comparison on Frame #0006

**Original / Baseline**
![Original](images/frame_0006.png)

**Method M**
![Method M](images/frame_0006M.png)

**Method L**
![Method L](images/frame_0006L.png)

**Proposed Method**
![Proposed](images/frame_0006P.png)
