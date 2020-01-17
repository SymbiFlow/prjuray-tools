#include <cassert>
#include <cstdio>
#include <prjxray/xilinx/xcupseries/ecc.h>

namespace prjxray {
namespace xilinx {
namespace xcupseries {

constexpr size_t kECCFrameNumber = 45;

uint64_t calculate_us_ecc(int word, int bit) {
  int nib = bit / 4;
  int nibbit = bit % 4;
  // ECC offset is expanded to 1 bit per nibble,
  // and then shifted based on the bit index in nibble
  // e.g. word 3, bit 9
  // offset: 0b10100110010 - concatenate (3 + (255 - 92)) [frame offset] and 9/4
  // [nibble offset] becomes: 0x10100110010 shifted by bit in nibble (9%4):
  // 0x20200220020
  uint32_t offset = (word + (255 - 92)) << 3 | nib;
  uint64_t exp_offset = 0;
  // Odd parity
  offset ^= (1 << 11);
  for (int i = 0; i < 11; i++)
    if (offset & (1 << i))
      offset ^= (1 << 11);
  // Expansion
  for (int i = 0; i < 12; i++)
    if (offset & (1 << i))
      exp_offset |= (1ULL << (4 * i));
  return exp_offset << nibbit;
};

// The ECC word for a given frame occupies 32-bits of word kECCFrameNumber
// and lower 16-bits of the next word, hence these bits must be masked
// during ECC computation
uint64_t get_us_ecc(uint32_t idx, uint32_t data, uint64_t ecc) {
  if (idx == kECCFrameNumber) {
    data = 0x0;
  }
  if (idx == kECCFrameNumber + 1) {
    data &= 0xFFFF0000;
  }
  for (int i = 0; i < 32; i++) {
    if (data & 1) {
      ecc ^= calculate_us_ecc(idx, i);
    }
    data >>= 1;
  }
  return ecc;
}

static uint64_t calculateECC(const std::vector<uint32_t> &data) {
  uint64_t ecc = 0;
  for (size_t ii = 0; ii < data.size(); ++ii) {
    ecc = get_us_ecc(ii, data[ii], ecc);
  }
  return ecc;
}

void updateECC(std::vector<uint32_t> &data) {
  assert(data.size() >= kECCFrameNumber);
  // Replace the old ECC with the new.
  uint64_t ecc(calculateECC(data));
  data[kECCFrameNumber] = ecc;
  data[kECCFrameNumber + 1] &= 0xffff0000;
  data[kECCFrameNumber + 1] |= (ecc >> 32) & 0xffff;
}

bool verifyECC(const absl::Span<const uint32_t> &data) {
  uint64_t ecc = 0;
  for (size_t ii = 0; ii < data.size(); ++ii) {
    ecc = get_us_ecc(ii, data[ii], ecc);
  }
  uint64_t original_ecc(
      static_cast<uint64_t>(data.at(kECCFrameNumber + 1) & 0xffff) << 32 |
      data.at(kECCFrameNumber));
  return original_ecc == ecc;
}

} // namespace xcupseries
} // namespace xilinx
} // namespace prjxray
