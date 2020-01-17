#ifndef PRJXRAY_LIB_XILINX_XC7SERIES_ECC_H_
#define PRJXRAY_LIB_XILINX_XC7SERIES_ECC_H_

#include <absl/types/span.h>
#include <cstdint>
#include <vector>

namespace prjxray {
namespace xilinx {
namespace xc7series {

// Extend the current ECC code with one data word (32 bit) at a given
// word index in the configuration frame and return the new ECC code.
uint32_t icap_ecc(uint32_t idx, uint32_t data, uint32_t ecc);

// Updates the ECC information in the frame.
void updateECC(std::vector<uint32_t> &data);

// Recalculate the ECC value and compare with the original value extracted from
// the frame's ECC word
bool verifyECC(const absl::Span<const uint32_t> &data);

} // namespace xc7series
} // namespace xilinx
} // namespace prjxray

#endif // PRJXRAY_LIB_XILINX_XC7SERIES_ECC_H_
