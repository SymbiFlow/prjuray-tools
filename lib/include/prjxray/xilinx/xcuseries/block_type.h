#ifndef PRJXRAY_LIB_XILINX_XCUSERIES_BLOCK_TYPE_H_
#define PRJXRAY_LIB_XILINX_XCUSERIES_BLOCK_TYPE_H_

#include <ostream>

#include <yaml-cpp/yaml.h>

namespace prjxray {
namespace xilinx {
namespace xcuseries {

enum class BlockType : unsigned int {
  CLB_IO_CLK = 0x0,
  BLOCK_RAM = 0x1,
  CFG_CLB = 0x2,
  /* reserved = 0x3, */
};

std::ostream &operator<<(std::ostream &o, BlockType value);

} // namespace xcuseries
} // namespace xilinx
} // namespace prjxray

namespace YAML {
template <> struct convert<prjxray::xilinx::xcuseries::BlockType> {
  static Node encode(const prjxray::xilinx::xcuseries::BlockType &rhs);
  static bool decode(const Node &node,
                     prjxray::xilinx::xcuseries::BlockType &lhs);
};
} // namespace YAML

#endif // PRJXRAY_LIB_XILINX_XCUSERIES_BLOCK_TYPE_H_
