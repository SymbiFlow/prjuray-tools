#include <prjxray/xilinx/frames.h>
#include <prjxray/xilinx/xc7series/ecc.h>
#include <prjxray/xilinx/xcupseries/ecc.h>
#include <prjxray/xilinx/xcuseries/ecc.h>

namespace prjxray {
namespace xilinx {
template <>
void Frames<Series7>::updateECC(typename Frames<Series7>::FrameData &data) {
  xc7series::updateECC(data);
}

template <>
void Frames<UltraScale>::updateECC(
    typename Frames<UltraScale>::FrameData &data) {
  xcuseries::updateECC(data);
}

template <>
void Frames<UltraScalePlus>::updateECC(
    typename Frames<UltraScalePlus>::FrameData &data) {
  xcupseries::updateECC(data);
}

// Spartan6 doesn't have ECC
template <>
void Frames<Spartan6>::updateECC(typename Frames<Spartan6>::FrameData &data) {}

template <> bool verifyECC<Series7>(const Series7::FrameData &data) {
  return xc7series::verifyECC(data);
}

template <> bool verifyECC<UltraScale>(const UltraScale::FrameData &data) {
  return xcuseries::verifyECC(data);
}

template <>
bool verifyECC<UltraScalePlus>(const UltraScalePlus::FrameData &data) {
  return xcupseries::verifyECC(data);
}

// Spartan6 doesn't have ECC
template <> bool verifyECC<Spartan6>(const Spartan6::FrameData &data) {
  return true;
}

} // namespace xilinx
} // namespace prjxray
