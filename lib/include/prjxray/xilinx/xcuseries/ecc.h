#ifndef PRJXRAY_LIB_XILINX_XCUSERIES_ECC_H_
#define PRJXRAY_LIB_XILINX_XCUSERIES_ECC_H_

#include <absl/types/span.h>
#include <cstdint>
#include <vector>

namespace prjxray {
namespace xilinx {
namespace xcuseries {

// Recalculate the ECC value and compare with the original value extracted from
// the frame's ECC word
bool verifyECC(const absl::Span<const uint32_t> &data);

// Updates the ECC information in the frame.
void updateECC(std::vector<uint32_t> &data);

} // namespace xcuseries
} // namespace xilinx
} // namespace prjxray

#endif // PRJXRAY_LIB_XILINX_XCUSERIES_ECC_H_
