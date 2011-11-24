#!/bin/bash

# this file is mostly meant to be used by the author himself.

ragel -G2 src/chunked_parser.rl
if [ $? != 0 ]; then
    echo 'Failed to generate the chunked parser.' 1>&2
    exit 1;
fi

root=`pwd`
version=$1
force=$2

ngx-build $force $version \
          --with-cc-opt="-funsigned-char" \
          --add-module=$root/../echo-nginx-module \
          --add-module=$root $opts \
          --with-debug
          #\
          #--with-http_ssl_module #\
          #--with-cc-opt="-pg" --with-ld-opt="-pg" \
  #--without-http_ssi_module  # we cannot disable ssi because echo_location_async depends on it (i dunno why?!)

