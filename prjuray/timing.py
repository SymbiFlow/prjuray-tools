# Copyright (C) 2020  The Project U-Ray Authors.
#
# Use of this source code is governed by a ISC-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/ISC
#
# SPDX-License-Identifier: ISC
from collections import namedtuple


class RcElement(namedtuple('RcElement', 'resistance capacitance')):
    """ One part of an RcNode, embedded within an RcTree.

    Attributes
    ----------

    resistance : float
        Resistance of element

    capacitance : float
        Capacitance of element

    """
    pass


class hashabledict(dict):
    """ Immutable version of dictionary with hash support. """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.hash = hash(tuple(sorted(self.items())))

    def setdefault(self, *args, **kwargs):
        raise NotImplementedError("hashabledict cannot be mutated.")

    def __setitem__(self, *args, **kwargs):
        raise NotImplementedError("hashabledict cannot be mutated.")

    def update(self, *args, **kwargs):
        raise NotImplementedError("hashabledict cannot be mutated.")

    def __hash__(self):
        return self.hash


def fast_slow_tuple_to_corners(arr):
    """ Convert delay 4-tuple into two IntristicDelay objects.

    Returns
    -------

    corners : dict of PvtCorner to IntristicDelay
        Dictionary keys of FAST and SLOW, mapping to the instrinsic delay
        for each corner.

    """

    fast_min, fast_max, slow_min, slow_max = map(float, arr)

    return hashabledict({
        PvtCorner.FAST:
        IntristicDelay(
            min=fast_min,
            max=fast_max,
        ),
        PvtCorner.SLOW:
        IntristicDelay(
            min=slow_min,
            max=slow_max,
        ),
    })
