#include <prjxray/xilinx/xcupseries/configuration_column.h>

namespace prjxray {
namespace xilinx {
namespace xcupseries {

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

} // namespace xcupseries
} // namespace xilinx
} // namespace prjxray

namespace xcupseries = prjxray::xilinx::xcupseries;

namespace YAML {

Node convert<xcupseries::ConfigurationColumn>::encode(
    const xcupseries::ConfigurationColumn &rhs) {
  Node node;
  node.SetTag("xilinx/xcupseries/configuration_column");
  node["frame_count"] = rhs.frame_count_;
  return node;
}

bool convert<xcupseries::ConfigurationColumn>::decode(
    const Node &node, xcupseries::ConfigurationColumn &lhs) {
  if (!node.Tag().empty() &&
      node.Tag() != "xilinx/xcupseries/configuration_column") {
    return false;
  }

  lhs.frame_count_ = node["frame_count"].as<unsigned int>();
  return true;
}

} // namespace YAML
