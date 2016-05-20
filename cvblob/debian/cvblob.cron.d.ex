#
# Regular cron jobs for the cvblob package
#
0 4	* * *	root	[ -x /usr/bin/cvblob_maintenance ] && /usr/bin/cvblob_maintenance
