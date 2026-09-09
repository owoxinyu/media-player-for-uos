#!/bin/sh

set -eu

sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-multimedia-dev \
    qt6-websockets-dev \
    ffmpeg
