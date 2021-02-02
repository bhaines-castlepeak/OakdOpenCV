//
//  @file util.hpp
//
//  Created by Bob Haines on 2/2/21
//
//

#include <opencv2/opencv.hpp>
#include "depthai/depthai.hpp"

/**
 * @brief Convert a depthai::ImageFrame to an OpenCV cv::Mat.
 * 
 * @param frame The frame to convert
 * @param data_type The format the ImgFrame is in.
 * @return cv::Mat Converted cv::Mat
 */
cv::Mat imgframe_to_mat(
    std::shared_ptr<dai::ImgFrame> frame, 
    int data_type=CV_8UC1
);
