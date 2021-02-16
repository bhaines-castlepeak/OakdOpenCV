//
//  util.cpp
//
//  Created by Bob Haines on 2/2/21
//
//

#include <opencv2/opencv.hpp>
#include "depthai/depthai.hpp"

// Convert a depthai::ImageFrame to an OpenCV cv::Mat.
cv::Mat oakdframe_to_cvmat(
    std::shared_ptr<dai::ImgFrame> frame, 
    int data_type=CV_8UC1
) {
    return cv::Mat(
        frame->getHeight(), 
        frame->getWidth(), 
        data_type, 
        frame->getData().data()
    );
}


