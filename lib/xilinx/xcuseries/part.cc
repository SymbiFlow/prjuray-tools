#include <prjxray/xilinx/xcuseries/part.h>

#include <iomanip>
#include <sstream>

namespace prjxray {
namespace xilinx {
namespace xcuseries {

absl::optional<Part> Part::FromFile(const std::string &path) {
  try {
    YAML::Node yaml = YAML::LoadFile(path);
    return yaml.as<Part>();
  } catch (YAML::Exception &e) {
    return {};
  }
}

bool Part::IsValidFrameAddress(FrameAddress address) const {
  auto addr_row = rows_.find(address.row());
  if (addr_row == rows_.end())
    return false;
  return addr_row->second.IsValidFrameAddress(address);
}

absl::optional<FrameAddress>
Part::GetNextFrameAddress(FrameAddress address) const {
  // Find the row for the current address.
  auto addr_row = rows_.find(address.row());

  // If the current address isn't in a known row, no way to know the next.
  if (addr_row == rows_.end())
    return {};

  // Ask the row for the next address.
  absl::optional<FrameAddress> next_address =
      addr_row->second.GetNextFrameAddress(address);
  if (next_address)
    return next_address;

  // The current row doesn't know what the next address is.  Assume that
  // the next valid address is the beginning of the next row.
  if (++addr_row != rows_.end()) {
    auto next_address =
        FrameAddress(address.block_type(), addr_row->first, 0, 0);
    if (addr_row->second.IsValidFrameAddress(next_address))
      return next_address;
  }

  // Block types are next numerically.
  if (address.block_type() < BlockType::BLOCK_RAM) {
    next_address = FrameAddress(BlockType::BLOCK_RAM, 0, 0, 0);
    if (IsValidFrameAddress(*next_address))
      return next_address;
  }

  if (address.block_type() < BlockType::CFG_CLB) {
    next_address = FrameAddress(BlockType::CFG_CLB, 0, 0, 0);
    if (IsValidFrameAddress(*next_address))
      return next_address;
  }

  return {};
}

} // namespace xcuseries
} // namespace xilinx
} // namespace prjxray

namespace xcuseries = prjxray::xilinx::xcuseries;

namespace YAML {

Node convert<xcuseries::Part>::encode(const xcuseries::Part &rhs) {
  Node node;
  node.SetTag("xilinx/xcuseries/part");

  std::ostringstream idcode_str;
  idcode_str << "0x" << std::hex << rhs.idcode_;
  node["idcode"] = idcode_str.str();
  node["rows"] = rhs.rows_;
  return node;
}

bool convert<xcuseries::Part>::decode(const Node &node, xcuseries::Part &lhs) {
  if (!node.Tag().empty() && node.Tag() != "xilinx/xcuseries/part")
    return false;

  if (!node["rows"] && !node["configuration_ranges"]) {
    return false;
  }

  lhs.idcode_ = node["idcode"].as<uint32_t>();
  if (node["rows"]) {
    lhs.rows_ = node["rows"].as<std::map<unsigned int, xcuseries::Row>>();
  } else if (node["configuration_ranges"]) {
    std::vector<xcuseries::FrameAddress> addresses;
    for (auto range : node["configuration_ranges"]) {
      auto begin = range["begin"].as<xcuseries::FrameAddress>();
      auto end = range["end"].as<xcuseries::FrameAddress>();
      for (uint32_t cur = begin; cur < end; ++cur) {
        addresses.push_back(cur);
      }
    }

    lhs = xcuseries::Part(lhs.idcode_, addresses);
  }

  return true;
};

} // namespace YAML
