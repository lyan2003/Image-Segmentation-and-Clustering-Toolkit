#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QStackedWidget>
#include <QGroupBox>
#include <opencv2/opencv.hpp>
#include <vector>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadClicked();
    void onApplyClicked();

private:
    QLabel *inputLabel;
    QLabel *outputLabel;
    QPushButton *btnLoad;
    QPushButton *btnApply;
    QComboBox *comboMainMode;
    QStackedWidget *stackedControls;

    QWidget *pageThresholding;
    QComboBox *comboThreshTechnique;
    QSpinBox *spinWindowSize;

    QWidget *pageSegmentation;
    QComboBox *comboSegTechnique;
    QSpinBox *spinK;
    QSpinBox *spinMaxIter;
    QDoubleSpinBox *spinTolerance;

    cv::Mat QImageToMat(QImage image);
    QImage MatToQImage(cv::Mat mat);

    // --- Thresholding Logic ---
    int optimalThresholdScratch(const cv::Mat& gray);
    int otsuThresholdScratch(const cv::Mat& gray);
    std::vector<int> spectralThresholdScratch(const cv::Mat& gray, int numModes = 3);    cv::Mat localThresholdScratch(const cv::Mat& gray, int windowSize);

    // --- Segmentation Logic ---
    cv::Mat kmeansScratch(const cv::Mat& img, int k, int maxIter);
    cv::Mat regionGrowingScratch(const cv::Mat& src, int tolerance);
    cv::Mat agglomerativeScratch(const cv::Mat& img, int k);
    cv::Mat meanShiftProcess(const cv::Mat& img, float spatialRad, float colorRad);};

#endif // MAINWINDOW_H
