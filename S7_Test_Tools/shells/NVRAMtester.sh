#!/bin/bash
### BEGIN INIT INFO
# Provides:			sentinel
# Required-Start:	$local_fs$ weston
# Required-Stop:	$local_fs$ weston
# Default-Start:	2 3 4 5
# Default-Stop: 	0 1 6
## END INIT INFO

SENTINEL_HOME=/opt/ncompass

executeCommand() { 
/usr/bin/weston-terminal --fullscreen --shell=/home/root/NVRAMTester;
}

case "$1" in
	start)
		. /etc/profile
		if [ -e /dev/mmcblk3p1 ]
		then
		    if [ -e /run/media/mmcblk3p1 ]
		    then
			if [ ! -h ${SENTINEL_HOME}/mediadisk ]
			then
			    ln -sf /run/media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk 
			    echo "ln -sf /run/media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk" >> ${SENTINEL_HOME}/mediadisk/nvram_log.txt
			fi
		    else
			mount /dev/mmcblk3p1 /media/mmcblk3p1
			if [ ! -h ${SENTINEL_HOME}/mediadisk ]
			then
			    ln -sf /media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk 
			    echo "ln -sf /media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk" >> ${SENTINEL_HOME}/mediadisk/nvram_log.txt
			fi
		    fi
		else
		    if [ ! -h ${SENTINEL_HOME}/mediadisk ]
		    then
			ln -sf /home/root ${SENTINEL_HOME}/mediadisk &>> ${SENTINEL_HOME}/mediadisk/nvram_log.txt
		    fi
		fi
		cd /home/root
		/bin/sleep 3 
		echo "#" >> ${SENTINEL_HOME}/mediadisk/nvram_log.txt
		echo "# Starting the NVRAMTester."  >> ${SENTINEL_HOME}/mediadisk/nvram_log.txt
		echo "#" >> ${SENTINEL_HOME}/mediadisk/nvram_log.txt
		executeCommand &>> ${SENTINEL_HOME}/mediadisk/nvram_log.txt & 
		#/usr/bin/weston-terminal --fullscreen --shell=/home/root/NVRAMTester &
		#/usr/bin/weston-terminal --fullscreen  &
		echo "#" >> ${SENTINEL_HOME}/mediadisk/nvram_log.txt
		echo "# Exiting the NVRAMTester start script." >> ${SENTINEL_HOME}/mediadisk/nvram_log.txt 
		echo "#" >> ${SENTINEL_HOME}/mediadisk/nvram_log.txt
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
