# OAK-D OpenCV Experiment

This is an experiment in getting OpenCV to interface with an OAK-D.  It is borrowed from another repo here:  <https://github.com/duncanrhamill/oakd_orbslam3>

`src/main.cpp` is describes what's needed in order to get left, right and disparity/depth images.

## Build

1. Install depthai-core dependencies and submodules git:

```#!/bin/bash
cd depthai-core
git submodule update --init --recursive
```

2. Make certain the depthai-core version is depthai-core @ f0c3f94.  This version is unreleased and undocumented but has interfaces for depthai-core to OpenCV.

3. Build the project:

```#!/bin/bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

4. From the root of this repo run the experiment (will need to create the video directory):

```#!/bin/bash
./build/bin/oakd_opencv --filename=test.avi --path=./video/
```
