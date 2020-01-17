#include <prjxray/xilinx/xcuseries/configuration_column.h>

namespace prjxray {
namespace xilinx {
namespace xcuseries {

bool ConfigurationColumn::IsValidFrameAddress(FrameAddress address) const {
  return address.minor() < frame_count_;
}

absl::optional<FrameAddress>
ConfigurationColumn::GetNextFrameAddress(FrameAddress address) const {
  if (!IsValidFrameAddress(address))
    return {};

  if (static_cast<unsigned int>(address.minor() + 1) < frame_count_) {
    return address + 1;
  }

  // Next address is not in this column.
  return {};
}

} // namespace xcuseries
} // namespace xilinx
} // namespace prjxray

namespace xcuseries = prjxray::xilinx::xcuseries;

namespace YAML {

Node convert<xcuseries::ConfigurationColumn>::encode(
    const xcuseries::ConfigurationColumn &rhs) {
  Node node;
  node.SetTag("xilinx/xcuseries/configuration_column");
  node["frame_count"] = rhs.frame_count_;
  return node;
}

bool convert<xcuseries::ConfigurationColumn>::decode(
    const Node &node, xcuseries::ConfigurationColumn &lhs) {
  if (!node.Tag().empty() &&
      node.Tag() != "xilinx/xcuseries/configuration_column") {
    return false;
  }

  lhs.frame_count_ = node["frame_count"].as<unsigned int>();
  return true;
}

} // namespace YAML
