//
//  @file main.cpp
//
//  Created by Bob Haines on 2/2/21
//
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>  // Video write
// FIX ME: missing disparity filter -- fix later.
// #include <opencv2/ximgproc/disparity_filter.hpp>
//#include "System.h"

#include "depthai/depthai.hpp"
#include "util.hpp"

// Camera frames per second
#define FramesPerSecond (20.0)
#define isColor (true)

// WLS parameters, taken from the OpenCV WLS filter docs recommended values.
#define WLS_LAMBDA (8000)
#define WLS_SIGMA (1.0)

// Items for parsing input parameters
const cv::String about =
    "Capture camera images, display and save video   \n"
    "Press <ESC> to exit                             \n"
    "Press <SPACE> to start or stop capturing video  \n"
    ;

const cv::String keys  =
    "{path       | ./video/      | path to save images. }"
    "{filename   | testvideo.avi | Output file name }"
    ;


int main(int argc, char *argv[]) {

    std::cout << "OAK-D/OpenCV Experiment" << std::endl;

    // get the video file output name using parser 
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about(about);
    
    if(argc < 2) {
        parser.printMessage();
        return 0;
    }
    
    std::string Path = parser.get<std::string>("path");
    std::string VideoName = parser.get<std::string>("filename");
    std::string FullFilename = Path + VideoName;
    std::string WindowName = "Video Image";
    cv::VideoWriter Video;
    cv::Mat Image;
    bool videoOn = false; // tracks if we are presently capturing images or not
    bool videoisOpen = false;
    // setup the video encoding format (system dependant)
    int FourCC = cv::VideoWriter::fourcc('M','J','P','G'); 
    
    if(!parser.check()) {
        parser.printErrors();
        return 0;
    }
   
    // Create the pipeline that we're going to build. Pipelines are depthai's
    // way of chaining up different series or parallel process, sort of like
    // gstreamer. 
    //
    // Our pipeline is going to extract the left and right rectified images
    // from the cameras so we can pass these into the SLAM system, as well as
    // disparity maps for building a point cloud.
    dai::Pipeline pipeline;

    // We need to create all the nodes in our pipeline, which are:
    //  - the left and right monochrome (greyscale) stereo cameras of the OAK-D
    //  - a stereo depth node, which generates disparity maps and rectified
    //    images. The disparity map will be used in constructing the global 
    //    point cloud, and the rectified images for SLAM tracking.
    //  - output nodes, which allow us to get the rectified image data and
    //    disparity map to use outside the pipeline.
    auto mono_left = pipeline.create<dai::node::MonoCamera>();
    auto mono_right = pipeline.create<dai::node::MonoCamera>();
    auto stereo = pipeline.create<dai::node::StereoDepth>();
    auto xout_rectif_left = pipeline.create<dai::node::XLinkOut>();
    auto xout_rectif_right = pipeline.create<dai::node::XLinkOut>();
    auto xout_disp = pipeline.create<dai::node::XLinkOut>();

    // And we set the names of each output node, so we can access them later as
    // output queues
    xout_rectif_left->setStreamName("rectified_left");
    xout_rectif_right->setStreamName("rectified_right");
    xout_disp->setStreamName("disparity");

    // Now we set which cameras are actually connected to the left and right
    // nodes, and set their resolution and framerate
    mono_left->setBoardSocket(dai::CameraBoardSocket::LEFT);
    mono_left->setResolution(
        dai::MonoCameraProperties::SensorResolution::THE_720_P
    );
    mono_left->setFps(20.0);
    mono_right->setBoardSocket(dai::CameraBoardSocket::RIGHT);
    mono_right->setResolution(
        dai::MonoCameraProperties::SensorResolution::THE_720_P
    );
    mono_right->setFps(FramesPerSecond);

    // Now we set the stereo node to output rectified images and disp maps. We
    // also set the rectify frames to not be mirrored, and to use black to fill
    // the edges of the rectified images. We need non-flipped images as we're
    // going to use them later down the line as input to the SLAM,
    // unfortunately this means our output disparity map will be flipped, so
    // we'll have to correct that later. We don't output depth as this would
    // disable the disparity map output.
    // 
    // We also enable extended disparity depth, which increases the maximum 
    // disparity and therefore provides a shorter minimum depth.
    stereo->setOutputRectified(true);
    stereo->setOutputDepth(false);
    stereo->setRectifyEdgeFillColor(0);
    stereo->setRectifyMirrorFrame(false);
    stereo->setExtendedDisparity(true);

    // We now link the cameras up to the stereo node
    mono_left->out.link(stereo->left);
    mono_right->out.link(stereo->right);

    // And the stereo rectified and disp outputs to the output nodes
    stereo->rectifiedLeft.link(xout_rectif_left->input);
    stereo->rectifiedRight.link(xout_rectif_right->input);
    stereo->disparity.link(xout_disp->input);

    // Now we can connect to the OAK-D device and start our pipeline
    dai::Device device(pipeline);
    device.startPipeline();

    // Finally to actually see the outputs we need to get their output queues
    // We use a max buffer size of 8 frames and set it into non-blocking mode.
    auto rectif_left_queue = device.getOutputQueue("rectified_left", 8, false);
    auto rectif_right_queue = device.getOutputQueue("rectified_right", 8, false);
    auto disp_queue = device.getOutputQueue("disparity", 8, false);
    
    // // Create the WLS (weighted least squares) filter, which we use to improve
    // // the quality of our disparity map. Also set the lambda and sigma values
    // auto wls_filter = cv::ximgproc::createDisparityWLSFilterGeneric(false);
    // wls_filter->setLambda(WLS_LAMBDA);
    // wls_filter->setSigmaColor(WLS_SIGMA);

    // To use OpenCV's reprojectImageTo3D we need a Q matrix, which is obtained
    // from stereoRectify. This means we'll have to extract some data from the
    // device itself, which is why this is done here. 
    // TODO: actually calculate these
    cv::Mat R1, R2, P1, P2, Q;

    // allocate a large image for a panel of sub-images
    cv::Mat PanelImage;

    // Now for the main loop
    while (true) {
        // Read the output frames from the OAK-D. These are blocking calls, so
        // they will wait until there's data available.
        auto rectif_left_frame = rectif_left_queue->get<dai::ImgFrame>();
        auto rectif_right_frame = rectif_left_queue->get<dai::ImgFrame>();
        auto disp_map_frame = disp_queue->get<dai::ImgFrame>();

        // Convert the frames into opencv images
        cv::Mat rectif_left = imgframe_to_mat(rectif_left_frame);
        cv::Mat rectif_right = imgframe_to_mat(rectif_right_frame);
        cv::Mat disp_map = imgframe_to_mat(disp_map_frame);

        // The raw disparity map is flipped, since we flipped the rectified
        // images, so we must flip it as well.
        cv::flip(disp_map, disp_map, 1);

        // // Filter the disparity map
        // cv::Mat filtered_disp_map;
        // wls_filter->filter(disp_map, rectif_right, filtered_disp_map);

        // Apply a colormap to the filtered disparity map, but don't normalise
        // it. Normalising the map will mean that the color doesn't correspond
        // directly with disparity.
        cv::Mat color_disparity;
        cv::applyColorMap(disp_map, color_disparity, cv::COLORMAP_JET);
        // cv::applyColorMap(filtered_disp_map, colour_disp, cv::COLORMAP_JET);
        cv::imshow("disparity", color_disparity);
        // cv::imshow("left", rectif_left);
        // cv::imshow("right", rectif_right);
        cv::hconcat(rectif_left, rectif_right, PanelImage);
        cv::imshow("stereo", PanelImage);

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
                // setup video file
                Video.open(Path + VideoName, FourCC, FramesPerSecond, color_disparity.size(), isColor);
                std::cout << "Saving output video with the following:" << std::endl;
                std::cout << "Video: " << Path + VideoName << std::endl;
                if (!Video.isOpened()) {
                    std::cout  << "Could not open the video file for writing." << std::endl;
                    return -1;
                } else {
                    std::cout << "Video file is open" << std::endl;
                    videoisOpen = true;
                }
            }
            // video is on, capture the frame
            Video << color_disparity;
        }
    }

    // close all the windows
    cv::destroyAllWindows();
    // if the video is open, close the video file
    if (videoisOpen) Video.release();

    return EXIT_SUCCESS;
}
