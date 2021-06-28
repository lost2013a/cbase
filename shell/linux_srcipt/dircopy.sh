#!/bin/bash
src_path='*.ko'
dest_path='/home/qlqcetc/nfs/v3s_drv/.'
#rm -rf $dest_path && cp -a $src_path  $dest_path && echo $0 "ok"
echo cp '$src_path.ko' to $dest_path
cp $src_path $dest_path

