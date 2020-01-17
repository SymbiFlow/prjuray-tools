#include <prjxray/xilinx/xcupseries/configuration_row.h>

namespace prjxray {
namespace xilinx {
namespace xcupseries {

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

} // namespace xcupseries
} // namespace xilinx
} // namespace prjxray

namespace xcupseries = prjxray::xilinx::xcupseries;

namespace YAML {

Node convert<xcupseries::Row>::encode(const xcupseries::Row &rhs) {
  Node node;
  node.SetTag("xilinx/xcupseries/row");
  node["configuration_buses"] = rhs.configuration_buses_;
  return node;
}

bool convert<xcupseries::Row>::decode(const Node &node, xcupseries::Row &lhs) {
  if (!node.Tag().empty() && node.Tag() != "xilinx/xcupseries/row") {
    return false;
  }

  lhs.configuration_buses_ =
      node["configuration_buses"]
          .as<std::map<xcupseries::BlockType, xcupseries::ConfigurationBus>>();
  return true;
}

} // namespace YAML
