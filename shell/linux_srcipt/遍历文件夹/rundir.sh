#！/bin/sh -
efile = '/mnt'
for file in /*
do
    if test -f $file
    then
        echo $file 是文件
    fi
	if test -d $file
    then
		if [ "$file" == '/tmp' ]; then
			echo $file 是不需要的目录
		else
			echo $file 是目录
		fi
    fi
done