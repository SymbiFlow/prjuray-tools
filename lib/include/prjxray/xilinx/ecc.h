#ifndef PRJXRAY_LIB_XILINX_ECC_H_
#define PRJXRAY_LIB_XILINX_ECC_H_

namespace prjxray {
namespace xilinx {

template <typename ArchType> bool is_ecc_bit(int i, int k) { return false; }

template <> bool is_ecc_bit<Series7>(int i, int k) {
  if (i == 50 && k <= 12) {
    return true;
  }
  return false;
}

template <> bool is_ecc_bit<UltraScalePlus>(int i, int k) {
  int halfword_index = 2 * i + k / 16;
  bool skip_usp_ecc = halfword_index >= 90 && halfword_index <= 92;
  return skip_usp_ecc;
}

template <> bool is_ecc_bit<UltraScale>(int i, int k) {
  if (i == 60 or (i == 61 && k <= 15)) {
    return true;
  }
  return false;
}
} // namespace xilinx
} // namespace prjxray

#endif // PRJXRAY_LIB_XILINX_ECC_H_
