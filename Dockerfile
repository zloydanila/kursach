FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    gdb \
    build-essential cmake ninja-build pkg-config \
    qtbase5-dev qttools5-dev qttools5-dev-tools \
    qtmultimedia5-dev libqt5multimedia5-plugins \
    libqt5websockets5-dev libqt5svg5-dev \
    libqt5sql5-psql libpq-dev libtag1-dev \
    gstreamer1.0-tools gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly gstreamer1.0-libav \
    gstreamer1.0-pulseaudio \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . /src

CMD ["bash","-lc","rm -rf /src/build && cmake -S /src -B /src/build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build /src/build && /src/build/Chorus"]
