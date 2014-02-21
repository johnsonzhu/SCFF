#!/bin/bash

if [ $# -lt 2 ];then
	echo "USAGE  : $0 <head_filename> <dest_dir>"
	echo "EXAMPLE: $0 aoitem_msg.h ~/my_test"
	exit 1
fi

cp -f $1 ./
cd auto_gen
chmod +x class_parser4web
sh gen.sh $1 $2
cd ..

