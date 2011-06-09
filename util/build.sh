#!/bin/bash

# this file is mostly meant to be used by the author himself.

ragel -G2 src/chunked_parser.rl

if [ $? != 0 ]; then
    echo 'Failed to generate the chunked parser.' 1>&2
    exit 1;
fi

root=`pwd`
version=$1
opts=$2
target=$root/work/nginx

if [ ! -d ./buildroot ]; then
    mkdir ./buildroot || exit 1
fi

cd buildroot || exit 1

if [ ! -s "nginx-$version.tar.gz" ]; then
    if [ -f ~/work/nginx-$version.tar.gz ]; then
        cp ~/work/nginx-$version.tar.gz ./ || exit 1
    else
        wget "http://sysoev.ru/nginx/nginx-$version.tar.gz" -O nginx-$version.tar.gz || exit 1
    fi

    tar -xzvf nginx-$version.tar.gz || exit 1
    cp $root/../no-pool-nginx/nginx-$version-no_pool.patch ./ || exit 1
    patch -p0 < nginx-$version-no_pool.patch || exit 1
fi

#tar -xzvf nginx-$version.tar.gz || exit 1
#cp $root/../no-pool-nginx/nginx-$version-no_pool.patch ./ || exit 1
#patch -p0 < nginx-$version-no_pool.patch || exit 1

cd nginx-$version/ || exit 1

if [[ "$BUILD_CLEAN" -eq 1 || ! -f Makefile || "$root/config" -nt Makefile || "$root/util/build.sh" -nt Makefile ]]; then
    ./configure --prefix=$target \
          --add-module=$root/../echo-nginx-module \
          --add-module=$root $opts \
          --with-debug
          #\
          #--with-http_ssl_module #\
          #--with-cc-opt="-pg" --with-ld-opt="-pg" \
  #--without-http_ssi_module  # we cannot disable ssi because echo_location_async depends on it (i dunno why?!)

fi
if [ -f $target/sbin/nginx ]; then
    rm -f $target/sbin/nginx
fi
if [ -f $target/logs/nginx.pid ]; then
    kill `cat $target/logs/nginx.pid`
fi
make -j3
make install

