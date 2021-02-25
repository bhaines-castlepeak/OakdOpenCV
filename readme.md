# OAK-D OpenCV Experiment

## Notes

This repo was built when the Luxonis version was 0.4, aka "Gen 1" of their software.  The depthai-core @ f0c3f94 is a beta version of their Gen 2 software.  I can tell all of their software is evolving rapidly and my guess is that most of this will change at some point.  However, depthai-core @ f0c3f94 seems to work.  

Another repo that might be helpful: I started to build this on my own and then found a similar application here: https://github.com/duncanrhamill/oakd_orbslam3

## running depthai demo beforehand

requirements for the OAK-D will download initialization data to the OAK-D that are needed for the OpenCV experiment to run.

```#!/bin/bash
git clone https://github.com/luxonis/depthai.git
cd depthai
python3 -m pip install -r requirements.txt
```

I recommend running the OAK-D demo to make sure everything is working on the DepthAI side after this.

```#!/bin/bash
python3 depthai_demo.py
```

it should pop up a window with live camera images.

## Build OAK-D OpenCV Experiment

1. Install depthai-core dependencies and submodules git:

```#!/bin/bash
cd cd [OakdOpenCV dir]/depthai-core
git submodule update --init --recursive
```

2. Make certain the depthai-core version is depthai-core @ f0c3f94.  This version is unreleased alpha and undocumented (at this point anyway) but has interfaces for depthai-core to OpenCV.

3. Build the project:

```#!/bin/bash
cd [OakdOpenCV dir]
mkdir build && cd build
cmake ..
make -j$(nproc)
```

4. Sample to run the experiment (user will need to create the video directory):

```#!/bin/bash
mkdir test
mkdir test/rgb
mkdir test/depth
./bin/oakd_opencv --path=./test
```
