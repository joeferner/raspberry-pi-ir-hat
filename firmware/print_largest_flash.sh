#!/bin/bash

nm --print-size --size-sort --radix=d build/firmware.elf | grep -v ' b '

