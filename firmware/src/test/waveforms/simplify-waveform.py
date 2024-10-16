#!/usr/bin/python3

import csv
import sys

with open(sys.argv[1], newline='') as csvfile:
    reader = csv.reader(csvfile)
    last_t = 0
    last_v = True
    for i, row in enumerate(reader):
        if i > 2:
            t = int(float(row[0]) * 1000 * 1000)
            if float(row[1]) > 1.5:
                v = 1
            else:
                v = 0
            if v != last_v:
                print("{},{},{}".format(t, v, t - last_t))
                last_v = v
                last_t = t
