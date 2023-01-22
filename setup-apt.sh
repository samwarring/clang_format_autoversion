#!/bin/sh -ex
apt-get update
apt-get install -y gpg wget lsb-release software-properties-common
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" > /etc/apt/sources.list.d/kitware.list
add-apt-repository -y ppa:ubuntu-toolchain-r/test
apt-get update
apt-get install -y                 \
    ninja-build                    \
    cmake                          \
    gcc                            \
    g++                            \
    gcc-aarch64-linux-gnu          \
    g++-aarch64-linux-gnu          \
    binutils-aarch64-linux-gnu

UBUNTU_VERSION=$(lsb_release -rs)
if [ "$UBUNTU_VERSION" = "18.04" ]; then
    # gcc version on ubuntu 18.04 is too old.
    apt-get install -y             \
        gcc-9                      \
        g++-9                      \
        gcc-8-aarch64-linux-gnu    \
        g++-8-aarch64-linux-gnu
fi