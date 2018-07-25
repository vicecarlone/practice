#!/bin/bash
for i in {1..500}; do
  xdotool mousemove 1131 312 click 1 & sleep 0.1 &
  xdotool mousemove 1042 369 click 1 & sleep 0.1 &
  xdotool mousemove 964 393 click 1 &
  sleep .2
done
