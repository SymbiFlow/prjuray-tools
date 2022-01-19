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

from collections import namedtuple
import enum


class BlockType(enum.Enum):
    # Frames describing CLB features, interconnect, clocks and IOs.
    CLB_IO_CLK = 'CLB_IO_CLK'

    # Frames describing block RAM initialization.
    BLOCK_RAM = 'BLOCK_RAM'


GridLoc = namedtuple('GridLoc', 'grid_x grid_y')
ClockRegion = namedtuple('ClockRegion', 'name x y')
GridInfo = namedtuple(
    'GridInfo',
    'bits sites prohibited_sites tile_type pin_functions clock_region')
BitAlias = namedtuple('BitAlias', 'tile_type start_offset sites')
Bits = namedtuple('Bits', 'base_address frames offset words alias')
BitsInfo = namedtuple('BitsInfo', 'block_type tile bits')
