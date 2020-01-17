#ifndef PRJXRAY_LIB_XILINX_XCUSERIES_PART_H_
#define PRJXRAY_LIB_XILINX_XCUSERIES_PART_H_

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <absl/types/optional.h>
#include <prjxray/xilinx/xcuseries/configuration_row.h>
#include <prjxray/xilinx/xcuseries/frame_address.h>
#include <yaml-cpp/yaml.h>

namespace prjxray {
namespace xilinx {
namespace xcuseries {

class Part {
public:
  constexpr static uint32_t kInvalidIdcode = 0;

  static absl::optional<Part> FromFile(const std::string &path);

  // Constructs an invalid part with a zero IDCODE. Required for YAML
  // conversion but shouldn't be used otherwise.
  Part() : idcode_(kInvalidIdcode) {}

  template <typename T>
  Part(uint32_t idcode, T collection)
      : Part(idcode, std::begin(collection), std::end(collection)) {}

  template <typename T> Part(uint32_t idcode, T first, T last);

  uint32_t idcode() const { return idcode_; }

  bool IsValidFrameAddress(FrameAddress address) const;

  absl::optional<FrameAddress> GetNextFrameAddress(FrameAddress address) const;

private:
  friend struct YAML::convert<Part>;

  uint32_t idcode_;
  std::map<unsigned int, Row> rows_;
};

template <typename T>
Part::Part(uint32_t idcode, T first, T last) : idcode_(idcode) {
  std::sort(first, last, [](const FrameAddress &lhs, const FrameAddress &rhs) {
    return lhs.row() < rhs.row();
  });

  for (auto row_first = first; row_first != last;) {
    auto row_last =
        std::upper_bound(row_first, last, row_first->row(),
                         [](const uint8_t &lhs, const FrameAddress &rhs) {
                           return lhs < rhs.row();
                         });

    rows_.emplace(row_first->row(), std::move(Row(row_first, row_last)));
    row_first = row_last;
  }
}

} // namespace xcuseries
} // namespace xilinx
} // namespace prjxray

namespace YAML {
template <> struct convert<prjxray::xilinx::xcuseries::Part> {
  static Node encode(const prjxray::xilinx::xcuseries::Part &rhs);
  static bool decode(const Node &node, prjxray::xilinx::xcuseries::Part &lhs);
};
} // namespace YAML

#endif // PRJXRAY_LIB_XILINX_XCUSERIES_PART_H_
