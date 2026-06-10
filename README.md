# Image Thresholding & Unsupervised Segmentation Studio

A cross-platform, high-performance C++ desktop application engineered using the Qt Framework and OpenCV core matrix representations. This toolkit implements modular analytical frameworks for intensity-based image thresholding and multi-class unsupervised segmentation, featuring global optimization models, multi-modal spectral histogram decomposition, adaptive local integral filtering, and advanced feature-space clustering algorithms.

---

## Technical Pipeline Architecture

The application design isolates compute-heavy iterative clustering loops and spatial neighborhood searches into dedicated algorithmic pipelines, utilizing Qt event loop processing mechanics to prevent graphical interface stuttering during macro-matrix operations.

```text
+-------------------------------------------------------------------------+
|                               QT GUI LAYER                              |
|   (Dynamic Control Panels, Stacked Widget Forms, Multi-Canvas Viewers)   |
+-------------------------------------------------------------------------+
                                    |
                                    v
+-------------------------------------------------------------------------+
|                     ALGORITHMIC TOOLKIT CORE (C++)                      |
|    [Thresholding Engine]  ---------->  [Unsupervised Seg Suite]         |
|  (Optimal, Otsu, Spectral, Local)     (K-Means, Region Grow, Shift, Agglo) |
+-------------------------------------------------------------------------+
                                    |
                                    v
+-------------------------------------------------------------------------+
|                           OPENCV MATRIX CORE                            |
|     (cv::Mat Buffers, Color Space Feature Vectors, Integral Matrices)   |
+-------------------------------------------------------------------------+

```

---

## Core Algorithmic Capabilities

### 1. Advanced Grayscale Image Thresholding

Implements custom, from-scratch parametric and non-parametric decision boundaries to binarize or multi-level partition grayscale scalar matrices:

* **Optimal (Iterative) Thresholding:** Executing a convergence loop that continuously bisects the grayscale spectrum. It computes background and foreground class means iteratively until the threshold stabilizes:

$$T_{t+1} = \frac{\mu_{\text{background}} + \mu_{\text{foreground}}}{2}$$

* **Otsu Global Variance Maximization:** Automatically determines the optimal global threshold by scanning the image histogram to maximize the inter-class variance metric, ensuring ideal separation between target foreground and background distributions:

$$\sigma_B^2(t) = \omega_0(t)\omega_1(t)\left[\mu_0(t) - \mu_1(t)\right]^2$$

* **Multi-Modal Spectral Thresholding:** Applies a 5-point moving average smoothing function over raw intensity histograms to suppress stochastic noise. It extracts dominant multi-modal peaks and evaluates deep valleys to compute multiple synchronized thresholds for complex scene partitioning (> 2 modes).
* **Local Adaptive Integral Filtering:** Computes dynamic pixel-specific thresholds based on neighborhood statistical means. Optimized via **Integral Images (Summed-Area Tables)** to reduce spatial kernel processing down to an absolute runtime efficiency of:

$$\mathcal{O}(1)$$

---

### 2. Unsupervised Multi-Class Image Segmentation

Integrates robust non-parametric and parametric clustering architectures to group color and gray feature vectors without prior class training:

* **K-Means Clustering Engine:** Partitions continuous BGR color spaces into $K$ distinct clusters through iterative Expectation-Maximization. Minimizes the squared Euclidean distance configurations between pixels and dynamically shifting centroids:

$$J = \sum_{i=1}^{k} \sum_{x \in S_i} \|x - \mu_i\|^2$$

* **Seed-Based Region Growing:** Propagates structural segments outward from high-intensity localized seed coordinate arrays. Employs an 8-connected depth-first search stack bounded strictly by scalar intensity tolerance matrices.
* **Agglomerative Hierarchical Clustering:** Executes a bottom-up hierarchical cluster tree that iteratively merges individual pixel vectors based on color-space proximity. Performance is optimized via controlled inter-matrix downscaling to prevent computational gridlocks.
* **Mean Shift Segmentation Processor:** Maps an iterative, non-parametric mode-seeking window across joint spatial-color feature domains. It shifts local window centers towards regions of maximum probability density, naturally isolating complex boundaries.

---

## Application Output Gallery

### 1. Grayscale Global & Local Thresholding

* **Optimal & Otsu Binary Extractions:**
<img width="1377" height="913" alt="image" src="https://github.com/user-attachments/assets/27b750eb-23f7-4706-9f1a-c0ef48d40634" />

* **Multi-Modal Spectral & Local Integral Maps:**
<img width="1377" height="913" alt="image" src="https://github.com/user-attachments/assets/9540181d-c996-49ef-8306-f41d5efce9e1" />
<img width="1377" height="913" alt="image" src="https://github.com/user-attachments/assets/5c74fb72-b6e3-4345-8774-6cd6d7c17f2d" />


### 2. Unsupervised Color Segmentation

* **K-Means & Agglomerative Feature Groupings:**
<img width="1377" height="913" alt="image" src="https://github.com/user-attachments/assets/aed28f94-d299-4c8a-964e-68d21300bd7b" />

* **Region Growing & Mean Shift Density Maps:**
<img width="1377" height="913" alt="image" src="https://github.com/user-attachments/assets/efba5884-a455-453d-b170-28972f79669d" />


---

## Repository Directory Tree

```text
project4-image-segmentation/
├── CMakeLists.txt                 # Master cross-platform build pipeline configurations
├── .gitignore                     # Excludes local compiler cache, .qtcreator configs, and build targets
├── main.cpp                       # Application bootstrap and primary GUI thread initialization
├── mainwindow.cpp                 # Master GUI layout logic, stylesheet injectors, and reactive signal routing
├── mainwindow.h                   # Structural UI slots, view matrix allocations, and window actions
├── thresholding.cpp               # Implementation of Optimal, Otsu, Spectral, and Integral Local algorithms
├── thresholding.h                 # Declarations and namespace parameters for grayscale binarization
├── clustering.cpp                 # Multi-threaded K-Means, Agglomerative, and Mean-Shift clustering engines
├── clustering.h                   # Classes and vector math templates for feature-space segmentations
├── regiongrowing.cpp              # 8-connected stack expansion and automatic seed assignment mechanics
└── regiongrowing.h                # Declarations for localized region propagation pipelines

```

---

## Toolchain Setup and Deployment

### Prerequisites

* **Build Environment:** CMake (Version 3.16 or higher).
* **Graphical Framework:** Qt Framework Core Modules (Qt5 / Qt6 Widgets distribution).
* **Matrix Dependency:** OpenCV Development Libraries (For `cv::Mat` structures and color conversions).
* **Compiler Requirements:** C++17 compliant compiler environment (GCC, Clang, or MinGW).

### Build Pipeline

1. Clone the repository and navigate into the source root:

```bash
git clone git@github.com:lyan2003/Image-Thresholding-Segmentation-Studio.git
cd Image-Thresholding-Segmentation-Studio

```

2. Initialize and configure an isolated build tracking tree:

```bash
mkdir build && cd build
cmake ..

```

3. Trigger the native compiler engine to compile and link all targets:

```bash
cmake --build .

```

4. Execute the resulting multi-module computer vision desktop application:

```bash
./ImageSegmentationStudio
