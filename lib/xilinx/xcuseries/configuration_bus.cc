#include <prjxray/xilinx/xcuseries/configuration_bus.h>

#include <iostream>

namespace prjxray {
namespace xilinx {
namespace xcuseries {

bool ConfigurationBus::IsValidFrameAddress(FrameAddress address) const {
  auto addr_column = configuration_columns_.find(address.column());
  if (addr_column == configuration_columns_.end())
    return false;

  return addr_column->second.IsValidFrameAddress(address);
}

absl::optional<FrameAddress>
ConfigurationBus::GetNextFrameAddress(FrameAddress address) const {
  // Find the column for the current address.
  auto addr_column = configuration_columns_.find(address.column());

  // If the current address isn't in a known column, no way to know the
  // next address.
  if (addr_column == configuration_columns_.end())
    return {};

  // Ask the column for the next address.
  absl::optional<FrameAddress> next_address =
      addr_column->second.GetNextFrameAddress(address);
  if (next_address)
    return next_address;

  // The current column doesn't know what the next address is.  Assume
  // that the next valid address is the beginning of the next column.
  if (++addr_column != configuration_columns_.end()) {
    auto next_address = FrameAddress(address.block_type(), address.row(),
                                     addr_column->first, 0);
    if (addr_column->second.IsValidFrameAddress(next_address))
      return next_address;
  }

  // Not in this bus.
  return {};
}

} // namespace xcuseries
} // namespace xilinx
} // namespace prjxray

namespace xcuseries = prjxray::xilinx::xcuseries;

namespace YAML {

Node convert<xcuseries::ConfigurationBus>::encode(
    const xcuseries::ConfigurationBus &rhs) {
  Node node;
  node.SetTag("xilinx/xcuseries/configuration_bus");
  node["configuration_columns"] = rhs.configuration_columns_;
  return node;
}

bool convert<xcuseries::ConfigurationBus>::decode(
    const Node &node, xcuseries::ConfigurationBus &lhs) {
  if (!node.Tag().empty() &&
      node.Tag() != "xilinx/xcuseries/configuration_bus") {
    return false;
  }

  lhs.configuration_columns_ =
      node["configuration_columns"]
          .as<std::map<unsigned int, xcuseries::ConfigurationColumn>>();
  return true;
}

} // namespace YAML
