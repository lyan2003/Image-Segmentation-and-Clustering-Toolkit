#ifndef REGIONGROWING_H
#define REGIONGROWING_H

#include <opencv2/opencv.hpp>

namespace Segmentation {
cv::Mat regionGrowingScratch(const cv::Mat& src, int tolerance);
}

#endif // REGIONGROWING_H
