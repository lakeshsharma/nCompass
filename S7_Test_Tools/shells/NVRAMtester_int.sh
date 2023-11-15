#!/bin/bash
### BEGIN INIT INFO
# Provides:			NVRAMTester
# Required-Start:	$local_fs$ weston
# Required-Stop:	$local_fs$ weston
# Default-Start:	2 3 4 5
# Default-Stop: 	0 1 6
## END INIT INFO

SENTINEL_HOME=/opt/ncompass
APPLIC_NAME=NVRAMTester
APPLIC_LOG_NAME=nvram
APPLICATION=/home/root/bin/${APPLIC_NAME}
LOG_FILE_PREFIX=${SENTINEL_HOME}/mediadisk/${APPLIC_LOG_NAME}

function is_readonly () {
    result=`grep " ro," /proc/mounts | grep $1 | wc -l`
    if [ $result -gt 0 ]
    then 
	echo "1";
    else
	echo "0";
    fi
}

function is_mounted () {
    result=`grep $1 /proc/mounts | wc -l`
    if [ $result -gt 0 ]
    then 
	echo "1";
    else
	echo "0";
    fi
}

function is_device () {
    if [ ! -e /dev/$1 ]
    then
	echo "0";
	return
    fi
    result=`ls /dev/$1 | wc -l`
    if [ $result -gt 0 ]
    then 
	echo "1";
    else
	echo "0";
    fi
}

function setupMountPoints() {
    if [ ! -d /opt/ncompass ]
    then
	echo "Creating /opt/ncompass as a needed mount point."
	mkdir /opt/ncompass
    fi

    if [ ! -d /media/mmcblk3p1 ]
    then
	echo "Creating /media/mmcblk3p1 as a needed mount point."
	mkdir /media/mmcblk3p1
    fi
}

function executeCommand() { 
    /usr/bin/weston-terminal --fullscreen --shell=${APPLICATION};
}

function executeMounts() {
#
# to see if a media card is available
    result=`is_device mmcblk3p1`
    if [ "$result" -eq "1" ]  
    then
    #
    # Check if sentinel has the card mounted as read only fatal error
    # run a fsck to make sure the mediadisk is clean
	#fsck -a  /dev/mmcblk3p1
	#
	result=`is_readonly mmcblk3p1`
	if [ "$result" -eq "1" ]
	then
	    echo "ERROR: /dev/mmcblk3p1 is read-only, exiting..."
	    exit -1
	fi
	if [ -e /run/media/mmcblk3p1 ]
	then
	    if [ -h ${SENTINEL_HOME}/mediadisk ]
	    then
		echo "Removing the existing soft link to mediadisk."
	    	rm ${SENTINEL_HOME}/mediadisk
	    fi
	    echo "ln -sf /run/media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk"
	    ln -sf /run/media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk
	else
	#
	# see if the mediadish has already been mounted
	    result=`is_mounted mmcblk3p1`
	    if [ "$result" -eq "0" ]
	    then
		echo "mounting mediacard at /media/mmcblk3p1."
		mount /dev/mmcblk3p1 /media/mmcblk3p1
	    fi
	    if [ -h ${SENTINEL_HOME}/mediadisk ]
	    then
	    	echo "Removing the existing soft link to mediadisk.."
		rm ${SENTINEL_HOME}/mediadisk;
	    fi
	    echo "ln -sf /media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk"
	    ln -sf /media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk
	fi
    else
    #
    # no media card found use root home directory for logs
	echo "ERROR: No Media card detected, using the /home/root for log files."
	if [ -h ${SENTINEL_HOME}/mediadisk ]
	then
	    echo "Removing the existing soft link to mediadisk..."
	    rm ${SENTINEL_HOME}/mediadisk
	fi
	echo "ln -sf /home/root ${SENTINEL_HOME}/mediadisk"
	ln -sf /home/root ${SENTINEL_HOME}/mediadisk
    fi
}

case "$1" in
	start)
		. /etc/profile
		#
		# make sure all directories exist
		setupMountPoints &> /home/root/mounting_log.txt
		#
		# make sure that everything is mounted and links to /op/ncompass/mediadisk
		executeMounts &>> /home/root/mounting_log.txt
		#
		# Make sure that none of the logfile are read-only to get around a reboot issue.
		if [ -e ${LOG_FILE_PREFIX}_log.txt ]
		then
		    echo "Making sure all files are not read-only" &>> /home/root/mounting_log.txt
		    chmod +rw  ${SENTINEL_HOME}/mediadisk/* &>> /home/root/mounting_log.txt
		fi
		#
		# Start the application
		cd /home/root
		/bin/sleep 5 
		echo "#" >> ${LOG_FILE_PREFIX}_log.txt
		echo "# Starting the $APPLIC_NAME."  >> ${LOG_FILE_PREFIX}_log.txt
		echo "#" >> ${LOG_FILE_PREFIX}_log.txt
		executeCommand &>> ${LOG_FILE_PREFIX}_log.txt & 
		echo "#" >> ${LOG_FILE_PREFIX}_log.txt
		echo "# Exiting the $APPLIC_NAME start script." >> ${LOG_FILE_PREFIX}_log.txt
		echo "#" >> ${LOG_FILE_PREFIX}_log.txt
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

