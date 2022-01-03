#!/bin/bash

if ! command -v serialport-list &> /dev/null; then
  echo "serialport-list could not be found, run npm install -g @serialport/list"
  exit 1
fi

serialport-list
