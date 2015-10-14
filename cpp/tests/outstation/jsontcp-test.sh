#!/bin/sh
SCRIPT=$(readlink -f $0)
SCRIPTPATH=`dirname $SCRIPT`

cat $SCRIPTPATH/jsontcp-sample.bin | nc localhost 3384
