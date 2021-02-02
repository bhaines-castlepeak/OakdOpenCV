//
//  @file util.cpp
//
//  Created by Bob Haines on 2/2/21
//
//

#include <opencv2/opencv.hpp>
#include "depthai/depthai.hpp"

cv::Mat imgframe_to_mat(
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


