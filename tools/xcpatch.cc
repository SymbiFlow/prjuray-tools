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

#include <iostream>

#include <gflags/gflags.h>
#include <prjxray/memory_mapped_file.h>
#include <prjxray/xilinx/architectures.h>
#include <prjxray/xilinx/bitstream_reader.h>
#include <prjxray/xilinx/bitstream_writer.h>
#include <prjxray/xilinx/configuration.h>

DEFINE_string(part_name, "", "");
DEFINE_string(part_file, "", "Definition file for target part.");
DEFINE_string(bitstream_file, "",
              "Initial bitstream to which the deltas are applied.");
DEFINE_string(
    frm_file, "",
    "File containing a list of frame deltas to be applied to the base "
    "bitstream.  Each line in the file is of the form: "
    "<frame_address> <word1>,...,<word101>.");
DEFINE_string(output_file, "", "Write patched bitsteam to file");
DEFINE_string(architecture, "Series7",
              "Architecture of the provided bitstream");

namespace xilinx = prjxray::xilinx;

template <typename ArchType>
int patch_frames(
    const std::string &frm_file_str,
    std::map<typename ArchType::FrameAddress, std::vector<uint32_t>> *frames,
    const absl::optional<typename ArchType::Part> &part) {
  xilinx::Frames<ArchType> frames_from_file;
  if (frames_from_file.readFrames(frm_file_str, *part)) {
    std::cerr << "Frames file " << frm_file_str << " not found or invalid"
              << std::endl;
    return 1;
  }

  // Apply the deltas.
  for (auto &frame : frames_from_file.getFrames()) {
    auto iter = frames->find(frame.first);
    if (iter == frames->end()) {
      std::cerr << "Bitstream doesn't contain frame with address 0x" << std::hex
                << static_cast<uint32_t>(frame.first) << std::endl;
      return 1;
    }

    auto &frame_data = iter->second;
    frame_data = frame.second;
  }

  return 0;
}

struct BitstreamPatcher {
  template <typename T> int operator()(T &arg) {
    using ArchType = std::decay_t<decltype(arg)>;
    auto part = ArchType::Part::FromFile(FLAGS_part_file);
    if (!part) {
      std::cerr << "Part file not found or invalid" << std::endl;
      return 1;
    }

    auto bitstream_file =
        prjxray::MemoryMappedFile::InitWithFile(FLAGS_bitstream_file);
    if (!bitstream_file) {
      std::cerr << "Can't open base bitstream file: " << FLAGS_bitstream_file
                << std::endl;
      return 1;
    }

    auto bitstream_reader = xilinx::BitstreamReader<ArchType>::InitWithBytes(
        bitstream_file->as_bytes());
    if (!bitstream_reader) {
      std::cout << "Bitstream does not appear to be compatible"
                   "with the current architecture!"
                << std::endl;
      return 1;
    }

    auto bitstream_config = xilinx::Configuration<ArchType>::InitWithPackets(
        *part, *bitstream_reader);
    if (!bitstream_config) {
      std::cerr << "Bitstream does not appear to be for this part" << std::endl;
      return 1;
    }

    // Copy the base frames to a mutable collection
    std::map<typename ArchType::FrameAddress, std::vector<uint32_t>> frames;
    for (auto &frame_val : bitstream_config->frames()) {
      auto &cur_frame = frames[frame_val.first];

      std::copy(frame_val.second.begin(), frame_val.second.end(),
                std::back_inserter(cur_frame));
    }

    if (!FLAGS_frm_file.empty()) {
      int ret = patch_frames<ArchType>(FLAGS_frm_file, &frames, part);
      if (ret != 0) {
        return ret;
      }
    }

    // Create data for the type 2 configuration packet with
    // information about all frames
    typename xilinx::Configuration<ArchType>::PacketData
        configuration_packet_data(
            xilinx::Configuration<ArchType>::createType2ConfigurationPacketData(
                frames, part));

    // Put together a configuration package
    typename ArchType::ConfigurationPackage configuration_package;
    xilinx::Configuration<ArchType>::createConfigurationPackage(
        configuration_package, configuration_packet_data, part);

    // Write bitstream.
    auto bitstream_writer =
        xilinx::BitstreamWriter<ArchType>(configuration_package);
    if (bitstream_writer.writeBitstream(configuration_package, FLAGS_part_name,
                                        FLAGS_frm_file, "xcpatch",
                                        FLAGS_output_file)) {
      std::cerr << "Failed to write bitstream" << std::endl
                << "Exitting" << std::endl;
    }
    return 0;
  }
};

int main(int argc, char *argv[]) {
  gflags::SetUsageMessage(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  xilinx::Architecture::Container arch_container =
      xilinx::ArchitectureFactory::create_architecture(FLAGS_architecture);
  return absl::visit(BitstreamPatcher(), arch_container);
}
