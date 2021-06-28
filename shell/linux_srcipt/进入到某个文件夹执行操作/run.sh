#!/bin/bash
CUR_DIR=$(cd `dirname $0`; pwd)
cd $CUR_DIR

docker-compose  -p bwsceb_srv down
docker-compose  -p bwsceb_srv up -d

chmod 777 -R /www/gky_ver/php/runtime
chmod 777 -R /bwsceb/tmp
mkdir -p /www/gky_ver/php/public/uploads/files/
chmod 777 -R /www/gky_ver/php/public/uploads/files/