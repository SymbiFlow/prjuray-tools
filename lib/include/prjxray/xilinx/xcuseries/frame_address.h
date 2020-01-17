#ifndef PRJXRAY_LIB_XILINX_XCUSERIES_FRAME_ADDRESS_H_
#define PRJXRAY_LIB_XILINX_XCUSERIES_FRAME_ADDRESS_H_

#include <cstdint>
#include <ostream>

#include <prjxray/xilinx/xcuseries/block_type.h>
#include <yaml-cpp/yaml.h>

#ifdef _GNU_SOURCE
#undef minor
#endif

namespace prjxray {
namespace xilinx {
namespace xcuseries {

enum Ranges {
  BLOCK_TYPE_HIGH = 25,
  BLOCK_TYPE_LOW = 23,

  ROW_HIGH = 22,
  ROW_LOW = 17,

  COLUMN_HIGH = 16,
  COLUMN_LOW = 7,

  MINOR_HIGH = 6,
  MINOR_LOW = 0
};

class FrameAddress {
public:
  FrameAddress() : address_(0) {}

  FrameAddress(uint32_t address) : address_(address){};

  FrameAddress(BlockType block_type, uint8_t row, uint16_t column,
               uint8_t minor);

  operator uint32_t() const { return address_; }

  BlockType block_type() const;
  bool is_bottom_half_rows() const;
  uint8_t row() const;
  uint16_t column() const;
  uint8_t minor() const;

private:
  uint32_t address_;
};

std::ostream &operator<<(std::ostream &o, const FrameAddress &addr);

} // namespace xcuseries
} // namespace xilinx
} // namespace prjxray

namespace YAML {
template <> struct convert<prjxray::xilinx::xcuseries::FrameAddress> {
  static Node encode(const prjxray::xilinx::xcuseries::FrameAddress &rhs);
  static bool decode(const Node &node,
                     prjxray::xilinx::xcuseries::FrameAddress &lhs);
};

} // namespace YAML

#endif // PRJXRAY_LIB_XILINX_XCUSERIES_FRAME_ADDRESS_H_
