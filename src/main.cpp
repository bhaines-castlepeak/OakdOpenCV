//
//  @file main.cpp
//
//  Created by Bob Haines on 2/2/21
//
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>  // Video write
#include <opencv2/ximgproc/disparity_filter.hpp>

// OAK-D depthai stuff
#include "depthai/depthai.hpp"

// simple OpenCV conversion from OAK-D's format to cv::Mat
#include "util.hpp"

// Camera frames per second
const double FramesPerSecond = 20.0;
const int isColor = true;

// WLS parameters, taken from the OpenCV WLS filter docs recommended values.
const int WLS_LAMBDA = 8000;
const double WLS_SIGMA = 1.0;

// Items for parsing input parameters
const cv::String about =
    "Capture camera images, display and save video and images  \n"
    "Press <ESC> to exit                                       \n"
    "Press <SPACE> to start or stop capturing video            \n"
    "note: setup the directories in advance.                   \n"
    ;

const cv::String keys  =
    "{path       | ./video        | path to save images.          }"
    "{rgbvideo   | rgbvideo.avi   | Output rgb video file name    }"
    "{depthvideo | depthvideo.avi | Output depth video file name  }"
    "{rgbdir     | rgb            | subdirectory for rgb images   }"
    "{depthdir   | depth          | subdirectory for depth images }"
    ;


int main(int argc, char *argv[]) {

    std::cout << "OAK-D/OpenCV Experiment" << std::endl;

    // setup the output directory, video files, and the image directories using parser 
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);
    
    if(argc < 2) {
        parser.printMessage();
        return 0;
    }
    
    std::string Path       = parser.get<std::string>("path") + "/";
    std::string rgbVideo   = Path + parser.get<std::string>("rgbvideo");
    std::string depthVideo = Path + parser.get<std::string>("depthvideo");
    std::string rgbDir     = Path + parser.get<std::string>("rgbdir") + "/";
    std::string depthDir   = Path + parser.get<std::string>("depthdir") + "/";
    cv::VideoWriter VideoRGB;
    cv::VideoWriter VideoD;
    cv::Mat Image;
    bool videoOn = false; // tracks if we are presently capturing images or not
    bool videoisOpen = false;
    // setup the video encoding format (system dependant)
    int FourCC = cv::VideoWriter::fourcc('M','J','P','G'); 
    
    if(!parser.check()) {
        parser.printErrors();
        return 0;
    }
   
    // Create the depthai pipeline:  
    // This behaves a bit like gstreamer
    //
    // This pipeline extracts the left and right rectified images 
    // and the disparity map for the right stereo image
    dai::Pipeline pipeline;

    // create pipeline nodes:
    //  - the left and right monochrome (greyscale) stereo cameras of the OAK-D
    //  - a stereo depth node (greyscale)
    //  - output nodes, which allow us to get the rectified image data and
    //    disparity map to use outside the pipeline.
    auto mono_left = pipeline.create<dai::node::MonoCamera>();
    auto mono_right = pipeline.create<dai::node::MonoCamera>();
    auto stereo = pipeline.create<dai::node::StereoDepth>();
    auto xout_rectif_left = pipeline.create<dai::node::XLinkOut>();
    auto xout_rectif_right = pipeline.create<dai::node::XLinkOut>();
    auto xout_disp = pipeline.create<dai::node::XLinkOut>();

    // set the names of each output node as an output queue
    xout_rectif_left->setStreamName("rectified_left");
    xout_rectif_right->setStreamName("rectified_right");
    xout_disp->setStreamName("disparity");

    // set cameras connected to the left and right nodes
    // and set resolution and framerate
    mono_left->setBoardSocket(dai::CameraBoardSocket::LEFT);
    mono_left->setResolution(dai::MonoCameraProperties::SensorResolution::THE_720_P);
    mono_left->setFps(20.0);
    mono_right->setBoardSocket(dai::CameraBoardSocket::RIGHT);
    mono_right->setResolution(dai::MonoCameraProperties::SensorResolution::THE_720_P);
    mono_right->setFps(FramesPerSecond);

    // set the stereo node to output rectified images and disp maps
    // use black to fill the edges of the rectified images
    // using non-flipped images 
    // non-flipped images == output disparity map will be flipped.  Correct
    // later.  Don't output depth -- that disables the disparity map output.
    // Enable extended disparity depth
    stereo->setOutputRectified(true);
    stereo->setOutputDepth(false);
    stereo->setRectifyEdgeFillColor(0);
    stereo->setRectifyMirrorFrame(false);
    stereo->setExtendedDisparity(true);

    // Link the cameras up to the stereo node
    mono_left->out.link(stereo->left);
    mono_right->out.link(stereo->right);

    // Stereo rectified and disp outputs to the output nodes
    stereo->rectifiedLeft.link(xout_rectif_left->input);
    stereo->rectifiedRight.link(xout_rectif_right->input);
    stereo->disparity.link(xout_disp->input);

    // Connect to the oakd and start pipeline
    dai::Device device(pipeline);
    device.startPipeline();

    // Get the output queues, use max buffer size of 8 frames
    // set into non-blocking mode
    auto rectif_left_queue = device.getOutputQueue("rectified_left", 8, false);
    auto rectif_right_queue = device.getOutputQueue("rectified_right", 8, false);
    auto disp_queue = device.getOutputQueue("disparity", 8, false);
    
    // Use WLS (weighted least squares) filter, to improve disparity quality
    auto wls_filter = cv::ximgproc::createDisparityWLSFilterGeneric(false);
    wls_filter->setLambda(WLS_LAMBDA);
    wls_filter->setSigmaColor(WLS_SIGMA);

    // Setup display images for camera operator feedback
    // allocate right image and color disparity image for the display
    cv::Mat color_right;
    cv::Mat filtered_disp_map;
    cv::Mat color_disparity;
    cv::Mat displayimage, disparityimage;

    // setup frame numbering for saving images
    int framenum;

    // Now for the main loop
    while (true) {
        // Blocking calls to read output frames from the oakd
        auto rectif_left_frame = rectif_left_queue->get<dai::ImgFrame>();
        auto rectif_right_frame = rectif_left_queue->get<dai::ImgFrame>();
        auto disp_map_frame = disp_queue->get<dai::ImgFrame>();

        // Convert the oakd frames into opencv images
        cv::Mat rectif_left = oakdframe_to_cvmat(rectif_left_frame);
        cv::Mat rectif_right = oakdframe_to_cvmat(rectif_right_frame);
        cv::Mat disp_map = oakdframe_to_cvmat(disp_map_frame);

        // The raw disparity map is flipped, since we flipped the rectified
        // images, so we must flip it as well.
        cv::flip(disp_map, disp_map, 1);

        // Filter the disparity map
        wls_filter->filter(disp_map, rectif_right, filtered_disp_map);

        // Apply a colormap to the filtered disparity map
        cv::applyColorMap(filtered_disp_map, color_disparity, cv::COLORMAP_JET);
        // convert the right image to color for display and to save it later
        cv::cvtColor(rectif_right, color_right, cv::COLOR_GRAY2BGR);

        // create display image with right image and disparity image
        cv::hconcat(color_right, color_disparity, displayimage);
        cv::imshow("right image + disparity", displayimage);

        // Display images and see if <ESC> or <SPACE> pressed
        char key = (char)cv::waitKey(1);
        // <ESC> then quit
        if (key == 27) break;
        // <SPACE> then toggle video on or off
        if (key == ' ') {
            videoOn = !videoOn;
        }

        if (videoOn) {
            // if video file hasn't been opened already, open it
            if (!videoisOpen) {
                // setup video file(s)
                VideoRGB.open(rgbVideo, FourCC, FramesPerSecond, color_right.size(), isColor);
                VideoD.open(depthVideo, FourCC, FramesPerSecond, filtered_disp_map.size(), false);
                std::cout << "Saving output video with the following:" << std::endl;
                std::cout << "VideoRGB: " << rgbVideo << std::endl;
                std::cout << "VideoD: " << depthVideo << std::endl;
                if (!VideoRGB.isOpened()) {
                    std::cout  << "Could not open the RGB video file for writing." << std::endl;
                    return -1;
                } else {
                    std::cout << "RGB video file is open" << std::endl;
                    videoisOpen = true;
                }
                if (!VideoD.isOpened()) {
                    std::cout  << "Could not open the depth video file for writing." << std::endl;
                    return -1;
                } else {
                    std::cout << "depth video file is open" << std::endl;
                    videoisOpen = true;
                }
            }
            // video is on, capture the frame
            VideoRGB << color_right;
            VideoD << filtered_disp_map;
            // convert framenum into padded string for file naming
            int numdigits = (int)ceil(log10(10000));
            std::stringstream ss;
            ss << std::setw(numdigits) << std::setfill('0') << framenum++;
            std::string index = ss.str();
            // save rgb and depth individual frames
            cv::imwrite(PathRGB + index + ".png", color_right);
            cv::imwrite(PathD + index + ".png", filtered_disp_map);
        }
    }

    // close all the windows
    cv::destroyAllWindows();
    // if the videos are open, close them
    if (videoisOpen) {
        VideoRGB.release();
        VideoD.release();
    }

    return EXIT_SUCCESS;
}
