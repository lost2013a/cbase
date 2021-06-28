#! /bin/sh

if [ "$1" == "" ]; then
	echo "parameter is none."
	exit 1
fi

mounted=`mount | grep -w $1 | wc -l`
# mounted, assume we umount

if [ $mounted -eq 1 ]; then
	udpsend remove $1 > /dev/null
	while true
	do
		/bin/umount "/media/$1" 2>>/dev/null
		sync
		mounted=`df | grep -w $1 | wc -l`
		if [ $mounted -eq 0 ]; then
			break
		fi
	done
	if ! rmdir "/media/$1"; then
		exit 1
	fi
	# not mounted, lets mount under /media
else
	if [[ `expr length $1` -eq 3 ]] && [[ `ls /dev/$1? | wc -l` -gt 0 ]]; then
		exit 1
	fi
	if [ `expr length $1` -eq 3 ]; then
		sleep 1
		if [[ `expr length $1` -eq 3 ]] && [[ `ls /dev/$1? | wc -l` -gt 0 ]]; then
			exit 1
		fi
	fi
	FSTYPE=`blkid /dev/$1 | sed "s/.*TYPE=\"\(.*\)\"/\1/"`
	if [ "$FSTYPE" == "" ]; then
		exit 1
	fi

	if ! mkdir -p "/media/$1"; then
		exit 1
	fi

	if [ "$FSTYPE" == "ntfs" ]; then
		FS=`cat /proc/filesystems | grep gntfs | awk '{print $1}'`
		if [ "$FS" == "gntfs"  ]; then
			mount -t gntfs "/dev/$1" "/media/$1" 2> /dev/null
		else
			ntfs-3g "/dev/$1" "/media/$1" 2> /dev/null
		fi
	elif [ "$FSTYPE" == "exfat" ]; then
		mount.exfat "/dev/$1" "/media/$1" 2> /dev/null &
	else
		mount -t vfat -o loop -o utf8=1 "/dev/$1" "/media/$1" 2> /dev/null
	fi

	if [ ! $? ]; then
		# failed to mount, clean up mountpoint
		rmdir "/media/$1"
		exit 1
	else
		udpsend add $1 > /dev/null
	fi
fi

exit 0
