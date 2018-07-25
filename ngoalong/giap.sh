#!/bin/bash
for i in {1..500}; do
  xdotool mousemove 1038 270 click 1 & sleep 0.1 &
  xdotool mousemove 1249 170 click 1 & sleep 0.1 &
  xdotool mousemove 1053 371 click 1 sleep 0.1 &
  xdotool mousemove 990 383 click 1 & 
  sleep .2
done
