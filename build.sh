#!/bin/bash

CPU_JOB_NUM=$(grep processor /proc/cpuinfo | awk '{field=$NF};END{print field+1}')
CLIENT=$(whoami)

KERNEL_CROSS_COMPILE_PATH="/opt/toolchains/arm-eabi-4.7/bin/arm-eabi-"

make -j$CPU_JOB_NUM ARCH=arm CROSS_COMPILE=$KERNEL_CROSS_COMPILE_PATH
