#include <prjxray/xilinx/xcupseries/frame_address.h>

#include <iomanip>

#include <prjxray/bit_ops.h>

namespace prjxray {
namespace xilinx {
namespace xcupseries {

FrameAddress::FrameAddress(BlockType block_type, uint8_t row, uint16_t column,
                           uint8_t minor) {
  address_ = bit_field_set(0, BLOCK_TYPE_HIGH, BLOCK_TYPE_LOW, block_type);

  address_ = bit_field_set(address_, ROW_HIGH, ROW_LOW, row);

  address_ = bit_field_set(address_, COLUMN_HIGH, COLUMN_LOW, column);

  address_ = bit_field_set(address_, MINOR_HIGH, MINOR_LOW, minor);
}

BlockType FrameAddress::block_type() const {
  return static_cast<BlockType>(
      bit_field_get(address_, BLOCK_TYPE_HIGH, BLOCK_TYPE_LOW));
}

bool FrameAddress::is_bottom_half_rows() const {
  return bit_field_get(address_, ROW_HIGH, ROW_HIGH);
}

uint8_t FrameAddress::row() const {
  return bit_field_get(address_, ROW_HIGH, ROW_LOW);
}

uint16_t FrameAddress::column() const {
  return bit_field_get(address_, COLUMN_HIGH, COLUMN_LOW);
}

uint8_t FrameAddress::minor() const {
  return bit_field_get(address_, MINOR_HIGH, MINOR_LOW);
}

std::ostream &operator<<(std::ostream &o, const FrameAddress &addr) {
  o << "[" << std::hex << std::showbase << std::setw(10)
    << static_cast<uint32_t>(addr) << "] "
    << " Row=" << std::setw(2) << std::dec
    << static_cast<unsigned int>(addr.row()) << " Column=" << std::setw(2)
    << std::dec << addr.column() << " Minor=" << std::setw(2) << std::dec
    << static_cast<unsigned int>(addr.minor()) << " Type=" << addr.block_type();
  return o;
}

} // namespace xcupseries
} // namespace xilinx
} // namespace prjxray

namespace YAML {

namespace xcupseries = prjxray::xilinx::xcupseries;

Node convert<xcupseries::FrameAddress>::encode(
    const xcupseries::FrameAddress &rhs) {
  Node node;
  node.SetTag("xilinx/xcupseries/frame_address");
  node["block_type"] = rhs.block_type();
  node["row"] = static_cast<unsigned int>(rhs.row());
  node["column"] = static_cast<unsigned int>(rhs.column());
  node["minor"] = static_cast<unsigned int>(rhs.minor());
  return node;
}

bool convert<xcupseries::FrameAddress>::decode(const Node &node,
                                               xcupseries::FrameAddress &lhs) {
  if (!(node.Tag() == "xilinx/xcupseries/frame_address" ||
        node.Tag() == "xilinx/xcupseries/configuration_frame_address") ||
      !node["block_type"] || !node["row"] || !node["column"] || !node["minor"])
    return false;

  lhs = prjxray::xilinx::xcupseries::FrameAddress(
      node["block_type"].as<xcupseries::BlockType>(),
      node["row"].as<unsigned int>(), node["column"].as<unsigned int>(),
      node["minor"].as<unsigned int>());
  return true;
}

} // namespace YAML
