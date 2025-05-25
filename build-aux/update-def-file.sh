#!/bin/bash

set -e

echo $1 $2

echo "EXPORTS" > "$2"
nm -gD "$1" | grep " T " | cut -f3 -d " " | sort | uniq >> "$2"
