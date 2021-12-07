#!/bin/bash

PORT=$1

picocom -b 57600 -f n -d 8 -y n -p 1 --echo --imap lfcrlf --omap crlf "${PORT}"
