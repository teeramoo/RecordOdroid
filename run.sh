#!/bin/bash

set -e


ctrl_c () {
	echo "** Trapped CTRL-C"
	#sudo ps -ef | grep "vidWrite" | grep -v grep | awk '{print $2}' | xargs kill
	sudo pkill -f recordVideo
	sudo pkill -f readIMU
	kill -TERM -$$
	exit
}

trap ctrl_c INT TERM
run_dir=`pwd`
cur_dt=`date +%Y-%m-%d:%H:%M:%S`

mkdir -p $run_dir'/OutputFiles/'

save_dir=$run_dir'/OutputFiles/'$cur_dt
imu_save_dir=$save_dir'/IMU.txt'
usb_port='/dev/ttyUSB0'

vid_save_dir=$save_dir'/'
image_width='640'
image_height='480'
video_fps='10.0'

mkdir $save_dir
echo $save_dir

cd VideoRecorder/build
./recordVideo $vid_save_dir $image_width $image_height $video_fps &

cd $run_dir

cd IMUrecorder/build
./readIMU $usb_port $imu_save_dir &

wait
