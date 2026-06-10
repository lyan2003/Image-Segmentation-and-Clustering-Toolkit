#ifndef THRESHOLDING_H
#define THRESHOLDING_H

#include <opencv2/opencv.hpp>
#include <vector>

namespace Thresholding {
int optimalThresholdScratch(const cv::Mat& gray);
int otsuThresholdScratch(const cv::Mat& gray);
std::vector<int> spectralThresholdScratch(const cv::Mat& gray, int numModes = 3);
cv::Mat localThresholdScratch(const cv::Mat& gray, int windowSize);
}

#endif // THRESHOLDING_H
