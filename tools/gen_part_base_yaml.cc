/*
 * Copyright (C) 2017-2020  The Project X-Ray Authors.
 * Copyright (C) 2020       The Project U-Ray Authors.
 *
 * Use of this source code is governed by a ISC-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/ISC
 *
 * SPDX-License-Identifier: ISC
 */

#include <libgen.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include <absl/strings/str_cat.h>
#include <absl/types/optional.h>
#include <absl/types/span.h>
#include <gflags/gflags.h>
#include <prjxray/memory_mapped_file.h>
#include <prjxray/xilinx/architectures.h>
#include <prjxray/xilinx/bitstream_reader.h>
#include <yaml-cpp/yaml.h>

DEFINE_bool(f, false, "Use FAR registers instead of LOUT ones");
DEFINE_string(architecture, "Series7",
              "Architecture of the provided bitstream");
namespace xilinx = prjxray::xilinx;

struct YamlWriter {
  YamlWriter(const absl::Span<uint8_t> &bytes) : bytes_(bytes) {}

  const absl::Span<uint8_t> &bytes_;

  template <typename T> int operator()(T &arg) {
    using ArchType = std::decay_t<decltype(arg)>;
    auto reader = xilinx::BitstreamReader<ArchType>::InitWithBytes(bytes_);
    if (!reader) {
      std::cerr << "Input doesn't look like a bitstream" << std::endl;
      return 1;
    }

    bool found_fdri_write = false;
    std::vector<typename ArchType::FrameAddress> frame_addresses;
    absl::optional<uint32_t> idcode;
    for (auto packet : *reader) {
      if (packet.opcode() !=
          xilinx::ConfigurationPacket<
              typename ArchType::ConfRegType>::Opcode::Write) {
        continue;
      }

      if (packet.address() == ArchType::ConfRegType::FDRI) {
        found_fdri_write = true;
      } else if ((packet.address() == ArchType::ConfRegType::IDCODE) &&
                 packet.data().size() == 1) {
        idcode = packet.data()[0];
      } else if (found_fdri_write &&
                 (packet.address() == ArchType::ConfRegType::LOUT) &&
                 (packet.data().size() == 1) && FLAGS_f == false) {
        frame_addresses.push_back(packet.data()[0]);
        found_fdri_write = false;
      } else if (found_fdri_write &&
                 (packet.address() == ArchType::ConfRegType::FAR) &&
                 (packet.data().size() == 1) && FLAGS_f == true) {
        frame_addresses.push_back(packet.data()[0]);
        found_fdri_write = false;
      }
    }

    if (!idcode) {
      std::cerr << "No IDCODE found." << std::endl;
      return 1;
    }

    if (frame_addresses.empty()) {
      std::cerr << "No LOUT or FAR writes found.  Was "
                << "BITSTREAM.GENERAL.DEBUGBITSTREAM or "
                << "BITSTREAM.GENERAL.PERFRAMECRC set to YES?" << std::endl;
      return 1;
    }

    auto part = typename ArchType::Part(*idcode, frame_addresses.begin(),
                                        frame_addresses.end());
    std::cout << YAML::Node(part) << std::endl;
    return 0;
  }
};

int main(int argc, char *argv[]) {
  gflags::SetUsageMessage(
      absl::StrCat("Usage: ", argv[0], " [bitfile] [options]"));
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (argc < 2) {
    std::cerr << "ERROR: no input specified" << std::endl;
    std::cerr << "Usage: " << basename(argv[0]) << " <bit_file>" << std::endl;
    return 1;
  }

  auto in_file_name = argv[1];
  auto in_file = prjxray::MemoryMappedFile::InitWithFile(in_file_name);
  if (!in_file) {
    std::cerr << "Unable to open bit file: " << in_file_name << std::endl;
    return 1;
  }
  auto in_bytes = absl::Span<uint8_t>(static_cast<uint8_t *>(in_file->data()),
                                      in_file->size());

  try {
    xilinx::Architecture::Container arch_container =
        xilinx::ArchitectureFactory::create_architecture(FLAGS_architecture);
    return absl::visit(YamlWriter(in_bytes), arch_container);
  } catch (absl::bad_variant_access &e) {
    std::cerr << "Error: Incorrect architecture. Supported architectures: "
                 "Series7, Spartan6, UltraScale, UltraScalePlus.\n";
  }
}
