/*
 * Copyright 2017-2022 F4PGA Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <fstream>
#include <iomanip>
#include <iostream>

#include <prjxray/xilinx/xcuseries/frame_address.h>

namespace xcuseries = prjxray::xilinx::xcuseries;

void xcu_frame_address_decode(std::istream *input_stream) {
  for (uint32_t frame_address_raw;
       (*input_stream) >> std::setbase(0) >> frame_address_raw;) {
    xcuseries::FrameAddress frame_address(frame_address_raw);
    std::cout << "[" << std::hex << std::showbase << std::setw(10)
              << frame_address_raw << "] "
              << " Row=" << std::setw(2) << std::dec
              << static_cast<unsigned int>(frame_address.row())
              << " Column=" << std::setw(2) << std::dec
              << frame_address.column() << " Minor=" << std::setw(2) << std::dec
              << static_cast<unsigned int>(frame_address.minor())
              << " Type=" << frame_address.block_type() << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    std::ifstream file_stream(argv[1]);
    if (file_stream) {
      xcu_frame_address_decode(&file_stream);
      return 0;
    }
  }

  xcu_frame_address_decode(&std::cin);

  return 0;
}
