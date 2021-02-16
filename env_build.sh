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
    gnupg \
    python3.6


# CMake
wget https://github.com/Kitware/CMake/releases/download/v3.19.3/cmake-3.19.3.tar.gz && \
    tar xf cmake-3.19.3.tar.gz && \
    cd cmake-3.19.3 && \
    ./configure && \
    make -j$(nproc)

sudo make install

cd && \
    rm -r cmake-3.19.3 && \
    rm cmake-3.19.3.tar.gz

# OpenCV 4.5
# install OpenCV dependencies
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
    libpcl-dev

# start OpenCV build
cd ~ && \
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
cd ~/opencv && \
    mkdir build && \
    cd build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
          -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib/modules \
          -D OPENCV_ENABLE_NONFREE=ON \
          -D BUILD_OPENCV_PYTHON3=TRUE .. && \
    make -j$(nproc)

sudo make install

    cd && \
    rm -r opencv && \
    rm -r opencv_contrib 

# Install DepthAI dependencies manually
# RUN wget -qO- http://docs.luxonis.com/_static/install_dependencies.sh | bash
sudo apt -y install \
    python3 \
    python3-pip \
    udev \
    git \
    python3-numpy \
    build-essential \
    libgtk2.0-dev \
    pkg-config \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    python-dev \
    libtbb2 \
    libtbb-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libdc1394-22-dev \
    ffmpeg \
    libsm6 \
    libxext6 \
    libgl1-mesa-glx \
    usbutils

# # https://github.com/luxonis/depthai/issues/295#issuecomment-757087753
# # add new sudo user
# ENV USERNAME depthai
# ENV HOME /home/$USERNAME
# RUN useradd -m $USERNAME && \
#         echo "$USERNAME:$USERNAME" | chpasswd && \
#         usermod --shell /bin/bash $USERNAME && \
#         usermod -aG sudo $USERNAME && \
#         mkdir /etc/sudoers.d && \
#         echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers.d/$USERNAME && \
#         chmod 0440 /etc/sudoers.d/$USERNAME && \
#         # Replace 1000 with your user/group id
#         usermod  --uid 1000 $USERNAME && \
#         groupmod --gid 1000 $USERNAME

# ENV DEBIAN_FRONTEND noninteractive
sudo apt install -y \
        less \
        emacs \
        build-essential \
        cmake \
        git \
        tmux \
        bash-completion \
        command-not-found \
        software-properties-common \
        xsel \
        xdg-user-dirs \
        wget \
        curl \
        usbutils \
        udev \
        && \
    apt-get clean && \
#     rm -rf /var/lib/apt/lists/*

# # https://github.com/luxonis/depthai-docker/blob/master/Dockerfile-depthai#L11
# RUN echo 'SUBSYSTEM=="usb", ATTRS{idVendor}=="03e7", MODE="0666"' | tee /etc/udev/rules.d/80-movidius.rules

# # USER $USERNAME
# WORKDIR /home/${USERNAME}
# SHELL ["/bin/bash", "-c"]
# RUN wget https://github.com/libusb/libusb/releases/download/v1.0.24/libusb-1.0.24.tar.bz2 && \
#     tar xf libusb-1.0.24.tar.bz2 && \
#     cd libusb-1.0.24 && \
#     ./configure --disable-udev --prefix="$PWD/install_dir" && \
#     make -j && \
#     make install
# ENV LD_LIBRARY_PATH="$PWD/libusb-1.0.24/install_dir/lib":$LD_LIBRARY_PATH

# RUN pip3 install setuptools
# RUN pip3 install scikit-build
# RUN apt-get install -y libusb-0.1-4

# RUN git clone https://github.com/luxonis/depthai.git
# RUN cd depthai && \
#     pip3 install -r requirements.txt
    