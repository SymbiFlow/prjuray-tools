#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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

from intervaltree import IntervalTree, Interval


class SegmentMap(object):
    def __init__(self, grid):
        self.segment_tree = IntervalTree()

        for bits_info in grid.iter_all_frames():
            self.segment_tree.add(
                Interval(
                    begin=bits_info.bits.base_address,
                    end=bits_info.bits.base_address + bits_info.bits.frames,
                    data=bits_info,
                ))

    def segment_info_for_frame(self, frame):
        """ Return all bits info that match frame address. """
        for frame in self.segment_tree[frame]:
            yield frame.data
