#ifndef CLUSTERING_H
#define CLUSTERING_H

#include <opencv2/opencv.hpp>

namespace Clustering {
cv::Mat kmeansScratch(const cv::Mat& img, int k, int maxIter);
cv::Mat agglomerativeScratch(const cv::Mat& img, int k);
cv::Mat meanShiftProcess(const cv::Mat& img, float spatialRad, float colorRad);
}

#endif // CLUSTERING_H
