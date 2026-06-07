#include "mainwindow.h"
#include <QCoreApplication>
#include <QDebug>
#include <cmath>
#include <algorithm>
#include <numeric>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ==========================================
    // 1. UI Setup & Initialization
    // Explanation: We build a dark-themed GUI dynamically.
    // We use a modular layout system (QHBoxLayout, QVBoxLayout)
    // to separate the control panel from the image viewing area.
    // ==========================================
    this->setWindowTitle("Image Studio - Task 5");
    this->resize(1100, 700);
    this->setStyleSheet("QMainWindow { background-color: #0d1117; color: white; }"
                        "QLabel { color: white; font-weight: bold; }"
                        "QGroupBox { color: white; border: 1px solid #30363d; border-radius: 5px; margin-top: 10px; }"
                        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 3px 0 3px; }"
                        "QPushButton { background-color: #21262d; color: white; border: 1px solid #30363d; border-radius: 5px; padding: 8px; }"
                        "QPushButton:hover { background-color: #30363d; }"
                        "QComboBox, QSpinBox, QDoubleSpinBox { background-color: #21262d; color: white; border: 1px solid #30363d; padding: 5px; }");

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    inputLabel = new QLabel("Input");
    inputLabel->setAlignment(Qt::AlignCenter);
    inputLabel->setStyleSheet("border: 2px solid #30363d; background-color: #161b22;");

    outputLabel = new QLabel("Output");
    outputLabel->setAlignment(Qt::AlignCenter);
    outputLabel->setStyleSheet("border: 2px solid #30363d; background-color: #161b22;");

    QVBoxLayout *inputLayout = new QVBoxLayout();
    inputLayout->addWidget(new QLabel("Input"), 0, Qt::AlignCenter);
    inputLayout->addWidget(inputLabel, 1);

    QVBoxLayout *outputLayout = new QVBoxLayout();
    outputLayout->addWidget(new QLabel("Output"), 0, Qt::AlignCenter);
    outputLayout->addWidget(outputLabel, 1);

    QHBoxLayout *imagesLayout = new QHBoxLayout();
    imagesLayout->addLayout(inputLayout);
    imagesLayout->addLayout(outputLayout);

    QVBoxLayout *controlsLayout = new QVBoxLayout();
    controlsLayout->setAlignment(Qt::AlignTop);
    controlsLayout->setSpacing(15);

    btnLoad = new QPushButton("Load Image");
    controlsLayout->addWidget(btnLoad);

    controlsLayout->addWidget(new QLabel("Choose The Mode:"));
    comboMainMode = new QComboBox();
    comboMainMode->addItems({"Thresholding", "Segmentation"});
    controlsLayout->addWidget(comboMainMode);

    // QStackedWidget is used here to dynamically switch between parameter panels
    // without cluttering the UI, enhancing user experience.
    stackedControls = new QStackedWidget();

    // ==========================================
    // -- Thresholding Settings Page --
    // ==========================================
    pageThresholding = new QWidget();
    QVBoxLayout *threshLayout = new QVBoxLayout(pageThresholding);
    threshLayout->addWidget(new QLabel("Thresholding Technique:"));
    comboThreshTechnique = new QComboBox();
    comboThreshTechnique->addItems({"Optimal", "Otsu", "Spectral", "Local"});
    threshLayout->addWidget(comboThreshTechnique);

    QGroupBox *groupThreshParams = new QGroupBox("Parameters");
    QVBoxLayout *threshParamsLayout = new QVBoxLayout(groupThreshParams);

    // Storing this label as a pointer so we can toggle its visibility later
    // specifically for the Local Thresholding technique.
    QLabel* lblWindowSize = new QLabel("Window Size (Local Only):");
    spinWindowSize = new QSpinBox();
    spinWindowSize->setRange(3, 99);
    spinWindowSize->setValue(15);
    spinWindowSize->setSingleStep(2);
    threshParamsLayout->addWidget(lblWindowSize);
    threshParamsLayout->addWidget(spinWindowSize);
    threshLayout->addWidget(groupThreshParams);
    threshLayout->addStretch();

    // ==========================================
    // -- Segmentation Settings Page --
    // ==========================================
    pageSegmentation = new QWidget();
    QVBoxLayout *segLayout = new QVBoxLayout(pageSegmentation);
    segLayout->addWidget(new QLabel("Segmentation Technique:"));
    comboSegTechnique = new QComboBox();
    comboSegTechnique->addItems({"K-Means", "Region Growing", "Agglomerative", "Mean Shift"});
    segLayout->addWidget(comboSegTechnique);

    QGroupBox *groupSegParams = new QGroupBox("Parameters");
    QVBoxLayout *segParamsLayout = new QVBoxLayout(groupSegParams);

    // Declaring labels locally to dynamically toggle their visibility and text
    // depending on the required parameters for each specific segmentation algorithm.
    QLabel* lblK = new QLabel("# of Classes (K):");
    spinK = new QSpinBox();
    spinK->setRange(2, 50);
    spinK->setValue(5);
    segParamsLayout->addWidget(lblK);
    segParamsLayout->addWidget(spinK);

    QLabel* lblMaxIter = new QLabel("Max Iterations:");
    spinMaxIter = new QSpinBox();
    spinMaxIter->setRange(1, 100);
    spinMaxIter->setValue(20);
    segParamsLayout->addWidget(lblMaxIter);
    segParamsLayout->addWidget(spinMaxIter);

    QLabel* lblTolerance = new QLabel("Tolerance:");
    spinTolerance = new QDoubleSpinBox();
    spinTolerance->setRange(0.001, 200.0);
    spinTolerance->setValue(40.0);
    segParamsLayout->addWidget(lblTolerance);
    segParamsLayout->addWidget(spinTolerance);

    segLayout->addWidget(groupSegParams);
    segLayout->addStretch();

    stackedControls->addWidget(pageThresholding);
    stackedControls->addWidget(pageSegmentation);
    controlsLayout->addWidget(stackedControls);

    btnApply = new QPushButton("Apply");
    controlsLayout->addWidget(btnApply);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addLayout(controlsLayout, 1);
    mainLayout->addLayout(imagesLayout, 4);

    // ==========================================
    // 2. Dynamic UI Logic (Show/Hide Parameters)
    // Explanation: Lambda functions are connected to combo boxes to update
    // the UI in real-time. This prevents users from entering irrelevant parameters.
    // ==========================================

    // Thresholding Dynamic UI mapping
    auto updateThreshUI = [=](const QString &tech) {
        bool isLocal = (tech == "Local");
        lblWindowSize->setVisible(isLocal);
        spinWindowSize->setVisible(isLocal);
        // Hide the entire parameter group box if the selected technique requires no parameters
        groupThreshParams->setVisible(isLocal);
    };
    connect(comboThreshTechnique, &QComboBox::currentTextChanged, updateThreshUI);
    updateThreshUI(comboThreshTechnique->currentText());

    // Segmentation Dynamic UI mapping
    auto updateSegUI = [=](const QString &tech) {
        if (tech == "K-Means") {
            lblK->show(); spinK->show();
            lblMaxIter->setText("Max Iterations:"); lblMaxIter->show(); spinMaxIter->show();
            lblTolerance->hide(); spinTolerance->hide();
        }
        else if (tech == "Region Growing") {
            lblK->hide(); spinK->hide();
            lblMaxIter->hide(); spinMaxIter->hide();
            lblTolerance->setText("Tolerance:"); lblTolerance->show(); spinTolerance->show();
        }
        else if (tech == "Agglomerative") {
            lblK->show(); spinK->show();
            lblMaxIter->hide(); spinMaxIter->hide();
            lblTolerance->hide(); spinTolerance->hide();
        }
        else if (tech == "Mean Shift") {
            lblK->hide(); spinK->hide();
            // Mean Shift utilizes spatial and color bandwidths rather than simple iterations
            lblMaxIter->setText("Spatial Radius:"); lblMaxIter->show(); spinMaxIter->show();
            lblTolerance->setText("Color Radius:"); lblTolerance->show(); spinTolerance->show();
        }
    };
    connect(comboSegTechnique, &QComboBox::currentTextChanged, updateSegUI);
    updateSegUI(comboSegTechnique->currentText());

    // Connect signals to slots for core functionality
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
    connect(btnApply, &QPushButton::clicked, this, &MainWindow::onApplyClicked);
    connect(comboMainMode, QOverload<int>::of(&QComboBox::currentIndexChanged), stackedControls, &QStackedWidget::setCurrentIndex);
}

MainWindow::~MainWindow() {}

void MainWindow::onLoadClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if(!image.isNull()) {
            inputLabel->setPixmap(QPixmap::fromImage(image).scaled(inputLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            outputLabel->clear();
            outputLabel->setText("Output");
        }
    }
}

// ---------------------------------------------------------
// Utility: Format Converters
// Explanation: Bridging Qt's QImage for display and OpenCV's cv::Mat for processing.
// ---------------------------------------------------------
cv::Mat MainWindow::QImageToMat(QImage image) {
    image = image.convertToFormat(QImage::Format_RGB888);
    return cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.bits(), image.bytesPerLine()).clone();
}

QImage MainWindow::MatToQImage(cv::Mat mat) {
    if(mat.channels() == 1) return QImage((const unsigned char*)(mat.data), mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    else return QImage((const unsigned char*)(mat.data), mat.cols, mat.rows, mat.step, QImage::Format_RGB888).copy();
}

// ==========================================
// 3. THRESHOLDING LOGIC
// ==========================================

// ---------------------------------------------------------
// Optimal (Iterative) Thresholding
// Explanation: Iteratively refines the threshold. It calculates the mean
// of background and foreground pixels, then updates the threshold as the
// midpoint of these two means until convergence (change < 1).
// ---------------------------------------------------------
int MainWindow::optimalThresholdScratch(const cv::Mat& gray) {
    //exact middle of grayscale
    int t_old = 127, t_new = 0;
    while(true) {
        // sumBg --> sum of background intensities
        //countBg --> number of background pixels
        long sumBg = 0, sumFg = 0; int countBg = 0, countFg = 0;
        for(int r = 0; r < gray.rows; r++) {
            for(int c = 0; c < gray.cols; c++) {
                int val = gray.at<uchar>(r,c);
                if(val < t_old) { sumBg += val; countBg++; } else { sumFg += val; countFg++; }
            }
        }
        int meanBg = (countBg == 0) ? 0 : sumBg / countBg;
        int meanFg = (countFg == 0) ? 0 : sumFg / countFg;
        //calculate new threshold
        t_new = (meanBg + meanFg) / 2;
        if(abs(t_new - t_old) < 1) break;
        t_old = t_new;
    }
    return t_new;
}

// ---------------------------------------------------------
// Otsu's Method
// Explanation: Analyzes the image histogram to find a single threshold
// that maximizes the inter-class variance (between-class variance) of
// background and foreground pixels.
// ---------------------------------------------------------
int MainWindow::otsuThresholdScratch(const cv::Mat& gray) {
    //initialize pixel intensities
    int hist[256] = {0};
    //make histogram
    for (int r = 0; r < gray.rows; r++)
        for (int c = 0; c < gray.cols; c++) hist[gray.at<uchar>(r, c)]++;

    int total = gray.rows * gray.cols;
    float sum = 0, sumB = 0, varMax = 0;
    int wB = 0, wF = 0, threshold = 0;
    //compute sum of intensity to all image
    for (int i = 0; i < 256; i++) sum += i * hist[i];
    // test i as a threshold
    for (int i = 0; i < 256; i++) {
        wB += hist[i]; if (wB == 0) continue;
        wF = total - wB; if  (wF == 0) break;
        sumB += (float)(i * hist[i]);
        float mB = sumB / wB; float mF = (sum - sumB) / wF;
        float varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);
        if (varBetween > varMax) { varMax = varBetween; threshold = i; }
    }
    return threshold;
}
// ---------------------------------------------------------
// Spectral Thresholding
// Explanation: Extracts dominant peaks from a smoothed histogram.
// It effectively thresholds by finding the "valleys" (minimum histogram
// values) strictly located between the extracted peaks.
// ---------------------------------------------------------
std::vector<int> MainWindow::spectralThresholdScratch(const cv::Mat& gray, int numModes) {
    std::vector<int> hist(256, 0);
    for (int r = 0; r < gray.rows; r++) {
        for (int c = 0; c < gray.cols; c++) {
            hist[gray.at<uchar>(r, c)]++;
        }
    }

    // Apply a simple 5-point moving average to smooth the histogram
    // preventing false peak detection due to noise.
    std::vector<int> smoothHist(256, 0);
    for(int i = 2; i < 254; i++) {
        smoothHist[i] = (hist[i-2] + hist[i-1] + hist[i] + hist[i+1] + hist[i+2]) / 5;
    }

    std::vector<int> peaks;
    std::vector<int> tempHist = smoothHist;

    // Iteratively extract the top 'numModes' peaks
    for(int m = 0; m < numModes; m++) {
        int maxVal = -1;
        int bestPeak = -1;

        for(int i = 0; i < 256; i++) {
            if(tempHist[i] > maxVal) {
                maxVal = tempHist[i];
                bestPeak = i;
            }
        }

        if(bestPeak != -1 && maxVal > 0) {
            peaks.push_back(bestPeak);
            // Erase a neighborhood around the detected peak to find the next distinct mode
            int radius = 20;
            int startErase = std::max(0, bestPeak - radius);
            int endErase = std::min(255, bestPeak + radius);
            for(int j = startErase; j <= endErase; j++) {
                tempHist[j] = 0;
            }
        } else {
            break;
        }
    }

    std::sort(peaks.begin(), peaks.end());
    std::vector<int> thresholds;

    // Fallback if not enough peaks are found
    if(peaks.size() < 2) {
        thresholds.push_back(127);
        return thresholds;
    }

    // Locate the deepest valley between each adjacent peak
    for(size_t i = 0; i < peaks.size() - 1; i++) {
        int start = peaks[i];
        int end = peaks[i+1];

        int minVal = smoothHist[start];
        int threshold = start;

        for(int j = start; j <= end; j++) {
            if(smoothHist[j] < minVal) {
                minVal = smoothHist[j];
                threshold = j;
            }
        }
        thresholds.push_back(threshold);
    }

    return thresholds;
}

// ---------------------------------------------------------
// Local Adaptive Thresholding (Using Integral Image)
// Explanation: Calculates the threshold locally for each pixel based on the
// mean of its neighborhood. Uses an Integral Image to drop time complexity
// to $O(1)$ per pixel, regardless of the window size.
// ---------------------------------------------------------
cv::Mat MainWindow::localThresholdScratch(const cv::Mat& gray, int windowSize) {
    cv::Mat out = cv::Mat::zeros(gray.size(), CV_8UC1);
    cv::Mat integral;
    cv::integral(gray, integral, CV_32S); // Compute Integral Image
    int offset = windowSize / 2;
    int C = 2; // Constant subtracted from the mean

    for(int r = 0; r < gray.rows; r++) {
        for(int c = 0; c < gray.cols; c++) {
            int r1 = std::max(r - offset, 0);
            int c1 = std::max(c - offset, 0);
            int r2 = std::min(r + offset, gray.rows - 1);
            int c2 = std::min(c + offset, gray.cols - 1);

            int count = (r2 - r1 + 1) * (c2 - c1 + 1);

            // $O(1)$ sum calculation using the integral image matrix
            int sum = integral.at<int>(r2+1, c2+1) - integral.at<int>(r1, c2+1) - integral.at<int>(r2+1, c1) + integral.at<int>(r1, c1);
            int mean = sum / count;

            out.at<uchar>(r, c) = (gray.at<uchar>(r, c) > (mean - C)) ? 255 : 0;
        }
    }
    return out;
}

// ==========================================
// 4. SEGMENTATION LOGIC
// ==========================================

// ---------------------------------------------------------
// K-Means Clustering Segmentation
// Explanation: Partitions the image into $K$ color clusters. Iteratively
// assigns each pixel to the nearest cluster centroid (in RGB space),
// then recalculates the centroids until the maximum iterations are reached.
// ---------------------------------------------------------
cv::Mat MainWindow::kmeansScratch(const cv::Mat& img, int k, int maxIter) {
    int rows = img.rows, cols = img.cols;

    // Step 1: Initialize cluster centers randomly
    std::vector<cv::Vec3b> centers(k);
    for(int i=0; i<k; i++) centers[i] = img.at<cv::Vec3b>(rand()%rows, rand()%cols);

    cv::Mat labels(rows, cols, CV_32S);

    // Step 2: Iterative Expectation-Maximization
    for(int iter=0; iter<maxIter; iter++) {
        // Expectation: Assign pixels to the nearest center
        for(int r=0; r<rows; r++) {
            for(int c=0; c<cols; c++) {
                double minDist = 1e9; int bestLabel = 0;
                cv::Vec3b px = img.at<cv::Vec3b>(r,c);
                for(int i=0; i<k; i++) {
                    double d = std::pow(px[0]-centers[i][0], 2) + std::pow(px[1]-centers[i][1], 2) + std::pow(px[2]-centers[i][2], 2);
                    if(d < minDist) { minDist = d; bestLabel = i; }
                }
                labels.at<int>(r,c) = bestLabel;
            }
        }

        // Maximization: Update centers based on current assignments
        std::vector<cv::Vec3d> newCenters(k, cv::Vec3d(0,0,0));
        std::vector<int> counts(k, 0);
        for(int r=0; r<rows; r++) {
            for(int c=0; c<cols; c++) {
                int l = labels.at<int>(r,c);
                newCenters[l] += (cv::Vec3d)img.at<cv::Vec3b>(r,c);
                counts[l]++;
            }
        }
        for(int i=0; i<k; i++) if(counts[i]>0) centers[i] = newCenters[i]/counts[i];

        // Keep UI responsive during heavy processing
        QCoreApplication::processEvents();
    }

    // Reconstruct the image using the final cluster centers
    cv::Mat output = img.clone();
    for(int r=0; r<rows; r++)
        for(int c=0; c<cols; c++) output.at<cv::Vec3b>(r,c) = centers[labels.at<int>(r,c)];
    return output;
}

// ---------------------------------------------------------
// Region Growing Segmentation
// Explanation: A region-based approach. We blur the image to reduce noise,
// automatically find a high-intensity seed, and iteratively "grow" the region
// outwards via an 8-connected neighborhood stack, bounded by an intensity tolerance.
// ---------------------------------------------------------
cv::Mat MainWindow::regionGrowingScratch(const cv::Mat& src, int tolerance) {
    cv::Mat blurred;
    cv::GaussianBlur(src, blurred, cv::Size(15, 15), 0); // Pre-processing to smooth noise

    cv::Mat dest = cv::Mat::zeros(src.size(), CV_8UC1);

    // Auto-seed selection: Search for a reasonably bright point
    cv::Point seed(blurred.cols/2, blurred.rows/2);
    for(int r=0; r<blurred.rows; r+=10) {
        for(int c=0; c<blurred.cols; c+=10) {
            if(blurred.at<uchar>(r,c) > 200) { seed = cv::Point(c,r); break; }
        }
    }

    // Depth First Search using a stack for region expansion
    std::vector<cv::Point> stack;
    stack.push_back(seed);
    uchar seedVal = blurred.at<uchar>(seed);

    while(!stack.empty()) {
        cv::Point p = stack.back(); stack.pop_back();
        // Check 8-connected neighbors
        for(int i=-1; i<=1; i++) {
            for(int j=-1; j<=1; j++) {
                cv::Point neighbor(p.x + i, p.y + j);
                if(neighbor.x>=0 && neighbor.y>=0 && neighbor.x<blurred.cols && neighbor.y<blurred.rows) {
                    // Include neighbor if unvisited and within intensity tolerance
                    if(dest.at<uchar>(neighbor) == 0 && abs(blurred.at<uchar>(neighbor) - seedVal) <= tolerance) {
                        dest.at<uchar>(neighbor) = 255;
                        stack.push_back(neighbor);
                    }
                }
            }
        }
    }
    return dest;
}

// ---------------------------------------------------------
// Agglomerative Hierarchical Clustering
// Explanation: A bottom-up approach where every pixel starts as its own cluster.
// We iteratively merge the two clusters with the minimum color distance.
// Downsampling is applied heavily to make the $O(N^3)$ computational cost viable.
// ---------------------------------------------------------
cv::Mat MainWindow::agglomerativeScratch(const cv::Mat& img, int k) {
    // Optimization: Downscale image to a very small working size
    int workSize = 50;
    cv::Mat smallImg;
    cv::resize(img, smallImg, cv::Size(workSize, workSize), 0, 0, cv::INTER_AREA);

    int n = smallImg.rows * smallImg.cols;
    std::vector<int> labels(n);
    std::vector<cv::Vec3f> clusterColors(n);

    // Initialize: Each pixel is a cluster
    for(int i=0; i<n; i++) {
        labels[i] = i;
        clusterColors[i] = smallImg.at<cv::Vec3b>(i / smallImg.cols, i % smallImg.cols);
    }

    int currentClusters = n;
    while(currentClusters > k) {
        double minDist = 1e9; int c1 = -1, c2 = -1;
        // Find the closest pair of clusters in feature space
        for(int i=0; i<n; i++) {
            if(labels[i] != i) continue; // Only process root nodes
            for(int j=i+1; j<n; j++) {
                if(labels[j] != j) continue;
                double d = cv::norm(clusterColors[i] - clusterColors[j]);
                if(d < minDist) { minDist = d; c1 = i; c2 = j; }
            }
        }

        // Merge cluster c2 into c1
        for(int i=0; i<n; i++) if(labels[i] == c2) labels[i] = c1;
        clusterColors[c1] = (clusterColors[c1] + clusterColors[c2]) / 2.0; // Update merged color
        currentClusters--;

        if(currentClusters % 100 == 0) QCoreApplication::processEvents(); // Prevent UI freeze
    }

    // Reconstruct the small image
    cv::Mat outSmall = smallImg.clone();
    for(int r=0; r<smallImg.rows; r++) {
        for(int c=0; c<smallImg.cols; c++) {
            int idx = r * smallImg.cols + c;
            int root = labels[idx];
            while(labels[root] != root) root = labels[root]; // Traverse to root
            outSmall.at<cv::Vec3b>(r,c) = clusterColors[root];
        }
    }

    // Upscale back to the original size
    cv::Mat finalOut;
    cv::resize(outSmall, finalOut, img.size(), 0, 0, cv::INTER_NEAREST);
    return finalOut;
}

// ---------------------------------------------------------
// Mean Shift Segmentation
// Explanation: A robust, non-parametric feature space analysis technique.
// For each pixel, it calculates the center of mass (mean) within a spatial
// and color radius, and "shifts" the pixel towards this mean until convergence.
// ---------------------------------------------------------
cv::Mat MainWindow::meanShiftProcess(const cv::Mat& img, float spatialRad, float colorRad) {
    cv::Mat res = img.clone();
    int rows = img.rows, cols = img.cols;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            float currR = r, currC = c;
            cv::Vec3f currColor = img.at<cv::Vec3b>(r, c);

            // Shift current point towards the localized mean
            for (int iter = 0; iter < 5; iter++) {
                float sumR = 0, sumC = 0, sumB = 0, sumG = 0, sumRed = 0;
                int count = 0;

                // Define bounding box bounded by Spatial Radius
                int rStart = std::max(0, (int)(currR - spatialRad));
                int rEnd = std::min(rows - 1, (int)(currR + spatialRad));
                int cStart = std::max(0, (int)(currC - spatialRad));
                int cEnd = std::min(cols - 1, (int)(currC + spatialRad));

                // Process pixels inside the spatial window
                for (int nr = rStart; nr <= rEnd; nr++) {
                    for (int nc = cStart; nc <= cEnd; nc++) {
                        cv::Vec3f neighborColor = img.at<cv::Vec3b>(nr, nc);

                        // Check if pixel falls inside the Color Radius feature space
                        float distSq = std::pow(currColor[0] - neighborColor[0], 2) +
                                       std::pow(currColor[1] - neighborColor[1], 2) +
                                       std::pow(currColor[2] - neighborColor[2], 2);

                        if (distSq <= colorRad * colorRad) {
                            sumR += nr; sumC += nc;
                            sumB += neighborColor[0]; sumG += neighborColor[1]; sumRed += neighborColor[2];
                            count++;
                        }
                    }
                }

                if (count == 0) break;

                // Calculate the new shifted center of mass
                float nextR = sumR / count, nextC = sumC / count;
                cv::Vec3f nextColor(sumB / count, sumG / count, sumRed / count);

                // Convergence check
                if (std::abs(nextR - currR) < 0.5 && std::abs(nextC - currC) < 0.5) break;

                currR = nextR; currC = nextC; currColor = nextColor;
            }
            res.at<cv::Vec3b>(r, c) = (cv::Vec3b)currColor; // Assign the mode
        }
        // UI Responsiveness mapping
        if (r % 10 == 0) QCoreApplication::processEvents();
    }
    return res;
}

// ==========================================
// 5. APPLICATION EXECUTION LOGIC
// ==========================================
void MainWindow::onApplyClicked() {
    if(inputLabel->pixmap().isNull()) { outputLabel->setText("Please Load an Image First!"); return; }

    outputLabel->setText("Processing...");
    QCoreApplication::processEvents();

    QImage qimg = inputLabel->pixmap().toImage();
    cv::Mat input = QImageToMat(qimg);
    cv::Mat gray, output;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);

    QString mainMode = comboMainMode->currentText();

    // Route execution flow based on user selection
    if (mainMode == "Thresholding") {
        QString tech = comboThreshTechnique->currentText();
        output = cv::Mat::zeros(gray.size(), CV_8UC1);

        if(tech == "Spectral") {
            int numModes = 3;
            std::vector<int> thresholds = spectralThresholdScratch(gray, numModes);

            // Apply multi-level thresholding based on the extracted modes
            int step = 255 / thresholds.size();
            for(int r = 0; r < gray.rows; r++) {
                for(int c = 0; c < gray.cols; c++) {
                    int val = gray.at<uchar>(r,c);
                    uchar outVal = 255;
                    for(size_t i = 0; i < thresholds.size(); i++) {
                        if(val <= thresholds[i]) {
                            outVal = i * step;
                            break;
                        }
                    }
                    output.at<uchar>(r,c) = outVal;
                }
            }
        }
        else if(tech == "Optimal" || tech == "Otsu") {
            int t = 0;
            if(tech == "Optimal") t = optimalThresholdScratch(gray);
            else t = otsuThresholdScratch(gray);

            for(int r = 0; r < gray.rows; r++)
                for(int c = 0; c < gray.cols; c++) output.at<uchar>(r,c) = (gray.at<uchar>(r,c) > t) ? 255 : 0;
        }
        else if(tech == "Local") {
            int wSize = spinWindowSize->value();
            if(wSize % 2 == 0) wSize++; // Ensure the window matrix has an absolute center (odd number)
            output = localThresholdScratch(gray, wSize);
        }
    }
    else if (mainMode == "Segmentation") {
        QString tech = comboSegTechnique->currentText();
        int k = spinK->value();
        int maxIter = spinMaxIter->value();
        double tol = spinTolerance->value();

        if(tech == "K-Means") output = kmeansScratch(input, k, maxIter);
        else if(tech == "Region Growing") output = regionGrowingScratch(gray, tol);
        else if(tech == "Agglomerative") output = agglomerativeScratch(input, k);
        else if(tech == "Mean Shift") output = meanShiftProcess(input, maxIter, tol);
    }

    // Render resulting matrix back to UI
    outputLabel->setPixmap(QPixmap::fromImage(MatToQImage(output)).scaled(outputLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
