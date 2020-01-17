#include <prjxray/xilinx/xcupseries/block_type.h>

namespace prjxray {
namespace xilinx {
namespace xcupseries {

std::ostream &operator<<(std::ostream &o, BlockType value) {
  switch (value) {
  case BlockType::CLB_IO_CLK:
    o << "CLB/IO/CLK";
    break;
  case BlockType::BLOCK_RAM:
    o << "Block RAM";
    break;
  case BlockType::CFG_CLB:
    o << "Config CLB";
    break;
  }

  return o;
}

} // namespace xcupseries
} // namespace xilinx
} // namespace prjxray

namespace YAML {

Node convert<prjxray::xilinx::xcupseries::BlockType>::encode(
    const prjxray::xilinx::xcupseries::BlockType &rhs) {
  switch (rhs) {
  case prjxray::xilinx::xcupseries::BlockType::CLB_IO_CLK:
    return Node("CLB_IO_CLK");
  case prjxray::xilinx::xcupseries::BlockType::BLOCK_RAM:
    return Node("BLOCK_RAM");
  case prjxray::xilinx::xcupseries::BlockType::CFG_CLB:
    return Node("CFG_CLB");
  default:
    return Node(static_cast<unsigned int>(rhs));
  }
}

bool YAML::convert<prjxray::xilinx::xcupseries::BlockType>::decode(
    const Node &node, prjxray::xilinx::xcupseries::BlockType &lhs) {
  auto type_str = node.as<std::string>();

  if (type_str == "CLB_IO_CLK") {
    lhs = prjxray::xilinx::xcupseries::BlockType::CLB_IO_CLK;
    return true;
  } else if (type_str == "BLOCK_RAM") {
    lhs = prjxray::xilinx::xcupseries::BlockType::BLOCK_RAM;
    return true;
  } else if (type_str == "CFG_CLB") {
    lhs = prjxray::xilinx::xcupseries::BlockType::CFG_CLB;
    return true;
  } else {
    return false;
  }
}

} // namespace YAML
