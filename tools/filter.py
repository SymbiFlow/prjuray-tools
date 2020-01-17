#!/usr/bin/env python3
# coding: utf-8
#
# Copyright 2020 Project U-Ray Authors
#
# Use of this source code is governed by a ISC-style
# license that can be found in the LICENSE file.
#
# SPDX-License-Identifier: ISC

import sys


def main():
    active = False

    with open(sys.argv[1]) as f:
        for line in f:
            sl = line.strip()
            if sl[0] == '.':
                active = sys.argv[2] in sl
            if active:
                print(sl)


if __name__ == "__main__":
    main()
