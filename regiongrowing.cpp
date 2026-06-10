#include "regiongrowing.h"
#include <vector>
#include <cmath>

namespace Segmentation {

cv::Mat regionGrowingScratch(const cv::Mat& src, int tolerance) {
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
                    if(dest.at<uchar>(neighbor) == 0 && std::abs(blurred.at<uchar>(neighbor) - seedVal) <= tolerance) {
                        dest.at<uchar>(neighbor) = 255;
                        stack.push_back(neighbor);
                    }
                }
            }
        }
    }
    return dest;
}

} // namespace Segmentation
