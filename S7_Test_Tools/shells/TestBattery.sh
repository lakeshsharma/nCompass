#!/bin/sh
### BEGIN INIT INFO
# Provides:			sentinel
# Required-Start:	$local_fs$ weston
# Required-Stop:	$local_fs$ weston
# Default-Start:	2 3 4 5
# Default-Stop: 	0 1 6
## END INIT INFO

SENTINEL_HOME=/opt/ncompass

case "$1" in
	start)
		. /etc/profile
		if [ -e /dev/mmcblk3p1 ]
		then
		    if [ -e /run/media/mmcblk3p1 ]
		    then
			echo "ln -sf /run/media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk" >> ${SENTINEL_HOME}/mediadisk/TestBattery_log.txt
			ln -sf /run/media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk
		    else
			mount /dev/mmcblk3p1 /media/mmcblk3p1
			echo "ln -sf /media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk" >> ${SENTINEL_HOME}/mediadisk/TestBattery_log.txt
			ln -sf /media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk
		    fi
		else
		    ln -sf /home/root ${SENTINEL_HOME}/mediadisk
		fi
		cd /home/root
		/bin/sleep 3 
		echo "#" >> ${SENTINEL_HOME}/mediadisk/TestBattery_log.txt
		echo "# Starting the TestBattery."  >> ${SENTINEL_HOME}/mediadisk/TestBattery_log.txt
		echo "#" >> ${SENTINEL_HOME}/mediadisk/TestBattery_log.txt
		/usr/bin/weston-terminal --fullscreen --shell=/home/root/TestBattery -l & 
		echo "#" >> ${SENTINEL_HOME}/mediadisk/TestBattery_log.txt
		echo "# Exiting the TestBattery start script." >> ${SENTINEL_HOME}/mediadisk/TestBattery_log.txt
		echo "#" >> ${SENTINEL_HOME}/mediadisk/TestBattery_log.txt
	;;

	stop)
		pkill weston-terminal;;

	restart)
		$0 stop
		sleep 5
		$0 start
	;;

	*)
		echo "usage: $0 ( start | stop | restart )"
	;;

esac
