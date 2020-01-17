#include <prjxray/xilinx/xcuseries/configuration_row.h>

namespace prjxray {
namespace xilinx {
namespace xcuseries {

bool Row::IsValidFrameAddress(FrameAddress address) const {
  auto addr_bus = configuration_buses_.find(address.block_type());
  if (addr_bus == configuration_buses_.end())
    return false;
  return addr_bus->second.IsValidFrameAddress(address);
}

absl::optional<FrameAddress>
Row::GetNextFrameAddress(FrameAddress address) const {
  // Find the bus for the current address.
  auto addr_bus = configuration_buses_.find(address.block_type());

  // If the current address isn't in a known bus, no way to know the next.
  if (addr_bus == configuration_buses_.end())
    return {};

  // Ask the bus for the next address.
  absl::optional<FrameAddress> next_address =
      addr_bus->second.GetNextFrameAddress(address);
  if (next_address)
    return next_address;

  // The current bus doesn't know what the next address is. Rows come next
  // in frame address numerical order so punt back to the caller to figure
  // it out.
  return {};
}

} // namespace xcuseries
} // namespace xilinx
} // namespace prjxray

namespace xcuseries = prjxray::xilinx::xcuseries;

namespace YAML {

Node convert<xcuseries::Row>::encode(const xcuseries::Row &rhs) {
  Node node;
  node.SetTag("xilinx/xcuseries/row");
  node["configuration_buses"] = rhs.configuration_buses_;
  return node;
}

bool convert<xcuseries::Row>::decode(const Node &node, xcuseries::Row &lhs) {
  if (!node.Tag().empty() && node.Tag() != "xilinx/xcuseries/row") {
    return false;
  }

  lhs.configuration_buses_ =
      node["configuration_buses"]
          .as<std::map<xcuseries::BlockType, xcuseries::ConfigurationBus>>();
  return true;
}

} // namespace YAML
