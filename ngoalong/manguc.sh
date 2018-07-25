#!/bin/bash
for i in {1..200}; do
  xdotool mousemove 1314 195 click 1 & sleep 0.1 &
  xdotool mousemove 1082 404 click 1 &
  sleep .2
done
