#! /bin/bash

ffmpeg -i $1 -vcodec mjpeg -q:v 1 -an $2
