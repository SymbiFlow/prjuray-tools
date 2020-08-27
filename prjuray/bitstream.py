#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (C) 2020  The Project U-Ray Authors.
#
# Use of this source code is governed by a ISC-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/ISC
#
# SPDX-License-Identifier: ISC

# Break frames into WORD_SIZE bit words.
WORD_SIZE_BITS = 16

# How many 16-bit words for frame in a US+ bitstream
FRAME_WORD_COUNT = 93 * 2

# What alignment is expect for columns.
FRAME_ALIGNMENT = 0x100
