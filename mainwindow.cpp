#include "mainwindow.h"
#include "thresholding.h"
#include "clustering.h"
#include "regiongrowing.h"
#include <QCoreApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ==========================================
    // 1. UI Setup & Initialization
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

    stackedControls = new QStackedWidget();

    // -- Thresholding Settings Page --
    pageThresholding = new QWidget();
    QVBoxLayout *threshLayout = new QVBoxLayout(pageThresholding);
    threshLayout->addWidget(new QLabel("Thresholding Technique:"));
    comboThreshTechnique = new QComboBox();
    comboThreshTechnique->addItems({"Optimal", "Otsu", "Spectral", "Local"});
    threshLayout->addWidget(comboThreshTechnique);

    QGroupBox *groupThreshParams = new QGroupBox("Parameters");
    QVBoxLayout *threshParamsLayout = new QVBoxLayout(groupThreshParams);

    QLabel* lblWindowSize = new QLabel("Window Size (Local Only):");
    spinWindowSize = new QSpinBox();
    spinWindowSize->setRange(3, 99);
    spinWindowSize->setValue(15);
    spinWindowSize->setSingleStep(2);
    threshParamsLayout->addWidget(lblWindowSize);
    threshParamsLayout->addWidget(spinWindowSize);
    threshLayout->addWidget(groupThreshParams);
    threshLayout->addStretch();

    // -- Segmentation Settings Page --
    pageSegmentation = new QWidget();
    QVBoxLayout *segLayout = new QVBoxLayout(pageSegmentation);
    segLayout->addWidget(new QLabel("Segmentation Technique:"));
    comboSegTechnique = new QComboBox();
    comboSegTechnique->addItems({"K-Means", "Region Growing", "Agglomerative", "Mean Shift"});
    segLayout->addWidget(comboSegTechnique);

    QGroupBox *groupSegParams = new QGroupBox("Parameters");
    QVBoxLayout *segParamsLayout = new QVBoxLayout(groupSegParams);

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
    // 2. Dynamic UI Logic
    // ==========================================
    auto updateThreshUI = [=](const QString &tech) {
        bool isLocal = (tech == "Local");
        lblWindowSize->setVisible(isLocal);
        spinWindowSize->setVisible(isLocal);
        groupThreshParams->setVisible(isLocal);
    };
    connect(comboThreshTechnique, &QComboBox::currentTextChanged, updateThreshUI);
    updateThreshUI(comboThreshTechnique->currentText());

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
            lblMaxIter->setText("Spatial Radius:"); lblMaxIter->show(); spinMaxIter->show();
            lblTolerance->setText("Color Radius:"); lblTolerance->show(); spinTolerance->show();
        }
    };
    connect(comboSegTechnique, &QComboBox::currentTextChanged, updateSegUI);
    updateSegUI(comboSegTechnique->currentText());

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

cv::Mat MainWindow::QImageToMat(QImage image) {
    image = image.convertToFormat(QImage::Format_RGB888);
    return cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.bits(), image.bytesPerLine()).clone();
}

QImage MainWindow::MatToQImage(cv::Mat mat) {
    if(mat.channels() == 1) return QImage((const unsigned char*)(mat.data), mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    else return QImage((const unsigned char*)(mat.data), mat.cols, mat.rows, mat.step, QImage::Format_RGB888).copy();
}

void MainWindow::onApplyClicked() {
    if(inputLabel->pixmap().isNull()) { outputLabel->setText("Please Load an Image First!"); return; }

    outputLabel->setText("Processing...");
    QCoreApplication::processEvents();

    QImage qimg = inputLabel->pixmap().toImage();
    cv::Mat input = QImageToMat(qimg);
    cv::Mat gray, output;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);

    QString mainMode = comboMainMode->currentText();

    if (mainMode == "Thresholding") {
        QString tech = comboThreshTechnique->currentText();
        output = cv::Mat::zeros(gray.size(), CV_8UC1);

        if(tech == "Spectral") {
            int numModes = 3;
            std::vector<int> thresholds = Thresholding::spectralThresholdScratch(gray, numModes);

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
            if(tech == "Optimal") t = Thresholding::optimalThresholdScratch(gray);
            else t = Thresholding::otsuThresholdScratch(gray);

            for(int r = 0; r < gray.rows; r++)
                for(int c = 0; c < gray.cols; c++) output.at<uchar>(r,c) = (gray.at<uchar>(r,c) > t) ? 255 : 0;
        }
        else if(tech == "Local") {
            int wSize = spinWindowSize->value();
            if(wSize % 2 == 0) wSize++;
            output = Thresholding::localThresholdScratch(gray, wSize);
        }
    }
    else if (mainMode == "Segmentation") {
        QString tech = comboSegTechnique->currentText();
        int k = spinK->value();
        int maxIter = spinMaxIter->value();
        double tol = spinTolerance->value();

        if(tech == "K-Means") output = Clustering::kmeansScratch(input, k, maxIter);
        else if(tech == "Region Growing") output = Segmentation::regionGrowingScratch(gray, tol);
        else if(tech == "Agglomerative") output = Clustering::agglomerativeScratch(input, k);
        else if(tech == "Mean Shift") output = Clustering::meanShiftProcess(input, maxIter, tol);
    }

    outputLabel->setPixmap(QPixmap::fromImage(MatToQImage(output)).scaled(outputLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
