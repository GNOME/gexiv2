#!/bin/bash

nm -gD "$1" | grep " T " | cut -f3 -d " " | sort | uniq > "$2"
