#
# Regular cron jobs for the raspicam package
#
0 4	* * *	root	[ -x /usr/bin/raspicam_maintenance ] && /usr/bin/raspicam_maintenance
