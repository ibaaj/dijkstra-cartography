#!/bin/bash

_sig() {
  kill -TERM $myPid 2>/dev/null
}
./osrm-routed planet-railway.osrm  &
myPid=$!
wait "$myPid"
