#!/bin/bash

# This file contains all of the cmdlines used to generate output
# for the test step in the CI pipeline.  It can also be used to
# regenerate reference output

set -x
set -e

# input/output directories:
traces=.gitlab-ci/traces
output=.gitlab-ci/out

# use the --update arg to update reference output:
if [ "$1" = "--update" ]; then
	output=.gitlab-ci/reference
fi

mkdir -p $output

# binary locations:
cffdump=./cffdump/cffdump
crashdec=./cffdump/crashdec

# helper to filter out paths that can change depending on
# who is building:
basepath=`dirname $0`
basepath=`dirname $basepath`
basepath=`pwd $basepath`
filter() {
	out=$1
	grep -vF "$basepath" > $out
}

#
# The Tests:
#

# dump only a single frame, and single tile pass, to keep the
# reference output size managable
$cffdump --frame 0 --once $traces/fd-clouds.rd.gz | filter $output/fd-clouds.log
$cffdump --frame 0 --once $traces/es2gears-a320.rd.gz | filter $output/es2gears-a320.log

# test a lua script to ensure we don't break scripting API:
$cffdump --script `dirname $cffdump`/scripts/parse-submits.lua $traces/shadow.rd.gz | filter $output/shadow.log

$crashdec -sf $traces/crash.devcore | filter $output/crash.log

