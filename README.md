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

<h2>🖼️ Visual and Quantitative Comparison on Frame #0006</h2>

<h3>📷 Visual Comparison</h3>
<table>
  <tr>
    <th>Original</th>
    <th>Proposed Method</th>
  </tr>
  <tr>
    <td><img src="images/frame_0006.png" width="300"/></td>
    <td><img src="images/frame_0006P.png" width="300"/></td>
  </tr>
  <tr>
    <th>Ma</th>
    <th>Lin</th>
  </tr>
  <tr>
    <td><img src="images/frame_0006M.png" width="300"/></td>
    <td><img src="images/frame_0006L.png" width="300"/></td>
  </tr>
</table>

<h3>📊 Quantitative Results</h3>
<table>
  <thead>
    <tr>
      <th>Method</th>
      <th>Hiding Capacity (bits/I-frame)</th>
      <th>Bitrate Increase (%)</th>
      <th>PSNR (dB)</th>
      <th>SSIM</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Ma</td>
      <td>1049.35</td>
      <td>3.24</td>
      <td>43.74</td>
      <td>0.9950</td>
    </tr>
    <tr>
      <td>Lin</td>
      <td>1966.20</td>
      <td>7.17</td>
      <td>39.95</td>
      <td>0.9854</td>
    </tr>
    <tr>
      <td><strong>Proposed</strong></td>
      <td><strong>3584.00</strong></td>
      <td><strong>9.90</strong></td>
      <td><strong>39.56</strong></td>
      <td><strong>0.9816</strong></td>
    </tr>
  </tbody>
</table>

<p><strong>Observation:</strong> Although our method shows a slightly lower PSNR/SSIM compared to Lin, it achieves <strong>1.79× more hiding capacity</strong> with a moderate bitrate increase, demonstrating a better trade-off between robustness, capacity, and visual quality in the compressed domain.</p>

### 📘 Implementation Details

This study implements the proposed information hiding method on top of the H.264 reference software **[JM (Joint Model)](https://iphome.hhi.de/suehring/tml/)**, with additional embedding and extraction modules integrated into the encoding and decoding processes, respectively. The experimental environment is detailed in Enviroment Table.

For encoder configuration, we enable **CAVLC** as the entropy coding mode. The **GOP (Group of Pictures)** size is set to 15 with a structure of `IBBBBBBBPBBBBBB`.

In our implementation, we utilize publicly available video sequences from the **[Xiph.org Video Test Media](https://media.xiph.org/video/derf/)** dataset.

For our experiments, we primarily select videos in **CIF resolution (352×288)**, with a frame rate of **30 fps** and **4:2:0 chroma subsampling**.

---

### Enviroment Table: Experimental Environment Specifications

| Item                 | Specification                      |
|----------------------|----------------------------------|
| Operating System      | Windows 10 64-bit                 |
| Development Platform  | Microsoft Visual Studio 2019     |
| Compiler             | MSVC                             |
| Reference Software    | JM 19.0 (H.264 Reference Encoder)|
| Programming Language  | C / C++                          |

---

### 🔍 Comparison with Existing Methods

This study compares the proposed method against existing approaches by [Ma et al.](https://ieeexplore.ieee.org/document/5559388) and [Lin et al.](https://www.sciencedirect.com/science/article/pii/S0164121212003020) The experiments primarily analyze four key metrics: **hiding capacity**, **bitrate increase**, **PSNR**, and **SSIM**, conducted under a fixed quantization parameter (**QP = 28**).
