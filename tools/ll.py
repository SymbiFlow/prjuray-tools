#!/usr/bin/env python3
# coding: utf-8
#
# Copyright 2020 Project U-Ray Authors
#
# Use of this source code is governed by a ISC-style
# license that can be found in the LICENSE file.
#
# SPDX-License-Identifier: ISC

import re
import sys


def main():
    line_re = re.compile(
        r'Bit\s+\d+\s+(0x[0-9A-Fa-f]+)\s+\d+\s+SLR\d\s+\d+\s+Block=([A-Za-z0-9_]+).*'
    )

    outlines = set()

    with open(sys.argv[1], 'r') as f:
        for line in f:
            m = line_re.match(line)
            if not m:
                continue
            frame = int(m.group(1), 16)
            site = m.group(2)
            bus = (frame >> 24) & 0x7
            half = (frame >> 23) & 0x1
            row = (frame >> 18) & 0x1F
            col = (frame >> 8) & 0x3FF
            minor = frame & 0xFF
            outlines.add("F=%08x B=%d H=%d R=%03d C=%04d M=%03d %s" %
                         (frame, bus, half, row, col, minor, site))

    for o in sorted(outlines):
        print(o)


if __name__ == "__main__":
    main()
