#!/bin/bash

PORT=$1

picocom -b 38400 -f n -d 8 -y n -p 1 --echo --imap lfcrlf "${PORT}"
