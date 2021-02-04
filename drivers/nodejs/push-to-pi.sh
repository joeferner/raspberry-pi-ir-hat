#!/bin/bash

PI_IP=$1
PI_DIR=$2

rsync -av --exclude node_modules . ${PI_IP}:${PI_DIR}
