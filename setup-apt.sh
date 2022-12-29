#!/bin/sh -ex
apt-get update
apt-get install -y gpg wget lsb-release software-properties-common
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" > /etc/apt/sources.list.d/kitware.list
add-apt-repository -y ppa:ubuntu-toolchain-r/test
apt-get update
apt-get install -y ninja-build cmake g++ gcc-9 g++-9