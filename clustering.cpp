#include "clustering.h"
#include <QCoreApplication>
#include <cmath>
#include <vector>
#include <algorithm>

namespace Clustering {

cv::Mat kmeansScratch(const cv::Mat& img, int k, int maxIter) {
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

cv::Mat agglomerativeScratch(const cv::Mat& img, int k) {
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

cv::Mat meanShiftProcess(const cv::Mat& img, float spatialRad, float colorRad) {
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

} // namespace Clustering
