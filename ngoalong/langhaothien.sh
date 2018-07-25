#!/bin/bash
for i in {1..50}; do
  xdotool mousemove 1173 223 click 1 & sleep 0.1 &
  xdotool mousemove 1077 373 click 1 & sleep 0.1 &
  xdotool mousemove 1044 272 click 1 sleep 0.1 &
  xdotool mousemove 956 394 click 1 & 
  sleep .2
done
