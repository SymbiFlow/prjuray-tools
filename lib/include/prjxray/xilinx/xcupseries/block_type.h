#ifndef PRJXRAY_LIB_XILINX_XCUPSERIES_BLOCK_TYPE_H_
#define PRJXRAY_LIB_XILINX_XCUPSERIES_BLOCK_TYPE_H_

#include <ostream>

#include <yaml-cpp/yaml.h>

namespace prjxray {
namespace xilinx {
namespace xcupseries {

enum class BlockType : unsigned int {
  CLB_IO_CLK = 0x0,
  BLOCK_RAM = 0x1,
  CFG_CLB = 0x2,
  /* reserved = 0x3, */
};

std::ostream &operator<<(std::ostream &o, BlockType value);

} // namespace xcupseries
} // namespace xilinx
} // namespace prjxray

namespace YAML {
template <> struct convert<prjxray::xilinx::xcupseries::BlockType> {
  static Node encode(const prjxray::xilinx::xcupseries::BlockType &rhs);
  static bool decode(const Node &node,
                     prjxray::xilinx::xcupseries::BlockType &lhs);
};
} // namespace YAML

#endif // PRJXRAY_LIB_XILINX_XCUPSERIES_BLOCK_TYPE_H_
