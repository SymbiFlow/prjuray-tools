#!/usr/bin/env python3
# coding: utf-8
#
# Copyright 2017-2022 F4PGA Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

import re
import sys
import json

# Usage: frames.txt tiles.txt tilegrid.json


def main():
    frame_line_re = re.compile(r'0x([0-9A-Fa-f]+).*')

    frame_rc_height = {}

    with open(sys.argv[1], 'r') as f:
        for line in f:
            m = frame_line_re.match(line)
            if not m:
                continue
            frame = int(m.group(1), 16)
            bus = (frame >> 24) & 0x7
            half = (frame >> 23) & 0x1
            row = (frame >> 18) & 0x1F
            col = (frame >> 8) & 0x3FF
            minor = frame & 0xFF
            if bus != 0 or half != 0:
                continue
            if (row, col) not in frame_rc_height:
                frame_rc_height[(row, col, frame & ~0xFF)] = minor + 1
            else:
                frame_rc_height[(row, col, frame & ~0xFF)] = max(
                    frame_rc_height[(row, col)], minor + 1)

    tiles_to_xy = {}
    with open(sys.argv[2], 'r') as tilef:
        for line in tilef:
            sl = line.strip().split(",")
            if len(sl) < 4:
                continue
            x = int(sl[0])
            y = int(sl[1])
            name = sl[2]
            tiles_to_xy[name] = (x, y)

    with open(sys.argv[3]) as tb_f:
        tbj = json.load(tb_f)

    frames_to_tiles = {}
    for tilename, tiledata in tbj.items():
        tile_offset = 0
        for chunk in tiledata:
            frame, start, size = chunk
            if frame not in frames_to_tiles:
                frames_to_tiles[frame] = []
            name = tilename.split(":")[0]
            frames_to_tiles[frame].append((start, tiles_to_xy[name][1],
                                           tiles_to_xy[name][0], name))
            tile_offset += size

    for frame, tiles in frames_to_tiles.items():
        tiles.sort()

    for rc, height in sorted(frame_rc_height.items()):
        row, col, frame = rc
        line = "%08x %6d %6d %6d" % (frame, row, col, height)
        print(line)
        frame = (row << 18) | (col << 8)
        last_start = 0
        if frame in frames_to_tiles and len(frames_to_tiles[frame]) > 0:
            for tile in frames_to_tiles[frame]:
                start, ty, tx, tname = tile
                print("                            %6d (%4d) %6d %6d %s" %
                      (start, start - last_start, tx, ty, tname))
                last_start = start


if __name__ == "__main__":
    main()
