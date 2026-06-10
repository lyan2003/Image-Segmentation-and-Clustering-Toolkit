#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QStackedWidget>
#include <QGroupBox>
#include <opencv2/opencv.hpp>

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
};

#endif // MAINWINDOW_H
