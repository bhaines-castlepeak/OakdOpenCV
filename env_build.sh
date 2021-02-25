# notes:
# I do not have CUDA (Nvidia GPU), so major libraries are built w/out CUDA support
sudo apt -y update && \
     apt -y upgrade

sudo apt -y install \
    gcc \
    clang \
    build-essential \
    libssl-dev \
    libtbb-dev \
    libeigen3-dev \
    curl \
    ca-certificates \
    apt-utils \
    git \
    wget \
    libboost-all-dev \
    libtiff-dev \
    libopenexr-dev \
    gnupg 
    
# install python 3.9
sudo apt -y install \
    software-properties-common
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt -y install \
    python3.9


# CMake
cd ~/Documents
wget https://github.com/Kitware/CMake/releases/download/v3.19.3/cmake-3.19.3.tar.gz && \
    tar xf cmake-3.19.3.tar.gz && \
    cd cmake-3.19.3 && \
    ./configure && \
    make -j$(nproc)

sudo make install

cd ~/Documents && \
    rm cmake-3.19.3.tar.gz

# OpenCV 4.5
# install OpenCV dependencies
cd ~/Documents
sudo apt -y install \
    build-essential \
    git \
    pkg-config \
    libgtk-3-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    libv4l-dev \
    libxvidcore-dev \
    libx264-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    gfortran \
    openexr \
    libatlas-base-dev \
    python3-dev \
    python3-numpy \
    libtbb2 \
    libtbb-dev \
    libdc1394-22-dev \
    libopenexr-dev \
    libgstreamer-plugins-base1.0-dev \
    libgstreamer1.0-dev \
    unzip \
    qtcreator \
    qt5-default \
    libpcl-dev \
    libavresample-dev \
    libopenjpip-server \
    libopenjp2-tools 
    
sudo pip install --upgrade pip

sudo snap install ffmpeg

# install gdal libraries for future
cd ~/Documents
sudo apt -y install \
    gdal-bin \
    libgdal-dev

# install QT for building OpenCV GUI apps
sudo apt -y install \
    qtcreator \
    qt5-default

# start OpenCV build
cd ~/Documents && \
    wget -O opencv.zip https://github.com/opencv/opencv/archive/4.5.1.zip && \
    wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.5.1.zip
# unpack
unzip opencv.zip && \
    unzip opencv_contrib.zip && \
    mv opencv-4.5.1 opencv && \
    mv opencv_contrib-4.5.1 opencv_contrib && \
    # clean up the zip files
    rm opencv.zip && \
    rm opencv_contrib.zip 
cd ~/Documents/opencv && \
    mkdir build && \
    cd build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
          -D OPENCV_EXTRA_MODULES_PATH=~/Documents/opencv_contrib/modules \
          -D OPENCV_ENABLE_NONFREE=ON \
          -D BUILD_OPENCV_PYTHON3=TRUE \
          -D WITH_QT=5 \
          -D WITH_QT=ON \
          -D OPENCV_GENERATE_PKGCONFIG=ON .. && \
    make -j$(nproc)

sudo make install

# build and install G2O libarary (RTAB recommended)
cd
sudo apt -y install libsuitesparse-dev
cd ~/Documents && \
   git clone https://github.com/RainerKuemmerle/g2o.git 
cd g2o
mkdir build
cd build
cmake -D BUILD_WITH_MARCH_NATIVE=OFF \
      -D G2O_BUILD_APPS=OFF \
      -D G2O_BUILD_EXAMPLES=OFF \
      -D G2O_USE_OPENGL=OFF ..
make -j$(nproc)
sudo make install

# Install GTSAM from Ubuntu PPA (RTAB recommended)
sudo add-apt-repository -y ppa:borglab/gtsam-release-4.0
sudo apt -y install libgtsam-dev libgtsam-unstable-dev

# Install Geogram (AliceVision requirement)
cd ~/Documents && \
    git clone --recursive https://github.com/alicevision/geogram.git
cd geogram
mkdir build && cd build
cmake \
  -D CMAKE_BUILD_TYPE=Release                 \
  -D GEOGRAM_WITH_TETGEN=OFF                  \
  -D GEOGRAM_WITH_HLBFGS=OFF                  \
  -D GEOGRAM_WITH_GRAPHICS=OFF                \
  -D GEOGRAM_WITH_EXPLORAGRAM=OFF             \
  -D GEOGRAM_WITH_LUA=OFF                     \
  -D VORPALINE_PLATFORM="Linux64-gcc-dynamic" \
  ..
make -j$(nproc)
sudo make install

# Install AliceVision (RTAB requirement)
sudo apt -y install \
    libpng-dev \
    libjpeg-dev \
    libtiff-dev \
    libxxf86vm1 \
    libxxf86vm-dev \
    libxi-dev \
    libxrandr-dev \
    graphviz \
    libeigen3-dev \
    libceres-dev \
    libflann-dev \
    libopenimageio-dev \
    liblapack-dev \
    liblapacke-dev \
    libatlas-base-dev \
    libblas-dev \
    liblapack-dev \
    libhdf5-dev
cd ~/Documents && \
    git clone --recursive https://github.com/alicevision/AliceVision.git
cd AliceVision
mkdir build && cd build
cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D ALICEVISION_USE_CUDA=OFF \
    -D AV_BUILD_CUDA=OFF \
    ..
make -j$(nproc)
sudo make install
sudo ldconfig

# # Install Intel RealSense
# sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-key F6E65AC044F831AC80A06380C8B3A55A6F3EFCDE
# sudo add-apt-repository -y "deb http://realsense-hw-public.s3.amazonaws.com/Debian/apt-repo focal main" -u
# sudo apt -y install librealsense2-dkms
# sudo apt -y install librealsense2-utils
# sudo apt -y install librealsense2-dev
# sudo apt -y install librealsense2-dbg


# build and install RTAB-Map
sudo apt -y update
sudo apt -y install \
    libsqlite3-dev \
    libpcl-dev \
    git \
    libproj-dev \
    libqt5svg5-dev \
    libusb-1.0-0-dev\
    libyaml-cpp-dev

cd ~/Documents && \
    git clone https://github.com/introlab/rtabmap.git rtabmap
cd rtabmap/build
cmake \
    -D WITH_ALICEVISION=ON \
    -D WITH_GTSAM=OFF \
    ..
make -j$(nproc)
sudo make install
sudo ldconfig
