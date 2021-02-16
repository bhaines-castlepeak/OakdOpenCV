# OAK-D OpenCV Experiment

This is an experiment in getting OpenCV to interface with an OAK-D.  

`main.cpp` is describes what's needed in order to get left, right and disparity/depth images.

## Build

1. Install depthai-core dependencies and submodules git:

```#!/bin/bash
cd depthai-core
git submodule update --init --recursive
```

2. Make certain the depthai-core version is depthai-core @ f0c3f94.  This version is unreleased and undocumented (at this point anyway) but has interfaces for depthai-core to OpenCV.

3. Build the project:

```#!/bin/bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

4. Sample to run the experiment (will need to create the video directory):

```#!/bin/bash
mkdir test
mkdir test/rgb
mkdir test/depth
./bin/oakd_opencv --path=./test
```
