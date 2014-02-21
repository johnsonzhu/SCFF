#!/bin/bash

#cp -f $1 ../ 2>/dev/null
filename="../`basename $1`"

echo "Parsing hpp ... $filename"
chmod +x class_parser4web
chmod +x d2u
./d2u $filename
rm -f tmp.xml
./class_parser4web 0x0001 $filename tmp.xml $2 
echo "Generating files ..."
chmod +x php447
./php447 -q auto_gen.php tmp.xml
#rm -f tmp.xml
#rm -f $filename

