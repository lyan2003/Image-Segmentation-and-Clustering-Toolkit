#include "thresholding.h"
#include <cmath>
#include <algorithm>

namespace Thresholding {

int optimalThresholdScratch(const cv::Mat& gray) {
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
        if(std::abs(t_new - t_old) < 1) break;
        t_old = t_new;
    }
    return t_new;
}

int otsuThresholdScratch(const cv::Mat& gray) {
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

std::vector<int> spectralThresholdScratch(const cv::Mat& gray, int numModes) {
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

cv::Mat localThresholdScratch(const cv::Mat& gray, int windowSize) {
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

            // O(1) sum calculation using the integral image matrix
            int sum = integral.at<int>(r2+1, c2+1) - integral.at<int>(r1, c2+1) - integral.at<int>(r2+1, c1) + integral.at<int>(r1, c1);
            int mean = sum / count;

            out.at<uchar>(r, c) = (gray.at<uchar>(r, c) > (mean - C)) ? 255 : 0;
        }
    }
    return out;
}

} // namespace Thresholding
