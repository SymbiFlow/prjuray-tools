#ifndef PRJXRAY_LIB_XILINX_ARCHITECTURES_H_
#define PRJXRAY_LIB_XILINX_ARCHITECTURES_H_

#include <absl/types/span.h>
#include <absl/types/variant.h>
#include <memory>
#include <vector>

#include <prjxray/xilinx/configuration_packet.h>
#include <prjxray/xilinx/spartan6/frame_address.h>
#include <prjxray/xilinx/spartan6/part.h>
#include <prjxray/xilinx/xc7series/frame_address.h>
#include <prjxray/xilinx/xc7series/part.h>
#include <prjxray/xilinx/xcupseries/frame_address.h>
#include <prjxray/xilinx/xcupseries/part.h>
#include <prjxray/xilinx/xcuseries/frame_address.h>
#include <prjxray/xilinx/xcuseries/part.h>

namespace prjxray {
namespace xilinx {

class Spartan6;
class Series7;
class UltraScale;
class UltraScalePlus;

class Architecture {
public:
  using Container =
      absl::variant<Series7, UltraScale, UltraScalePlus, Spartan6>;
  using FrameData = absl::Span<const uint32_t>;
  Architecture(const std::string &name) : name_(name) {}
  const std::string &name() const { return name_; }
  virtual ~Architecture() {}

private:
  const std::string name_;
};

class Spartan6 : public Architecture {
public:
  using ConfRegType = Spartan6ConfigurationRegister;
  using Part = spartan6::Part;
  using ConfigurationPackage =
      std::vector<std::unique_ptr<ConfigurationPacket<ConfRegType>>>;
  using FrameAddress = spartan6::FrameAddress;
  using WordType = uint16_t;
  Spartan6() : Architecture("Spartan6") {}
  static constexpr int words_per_frame = 65;
};

class Series7 : public Architecture {
public:
  using ConfRegType = Series7ConfigurationRegister;
  using Part = xc7series::Part;
  using ConfigurationPackage =
      std::vector<std::unique_ptr<ConfigurationPacket<ConfRegType>>>;
  using FrameAddress = xc7series::FrameAddress;
  using WordType = uint32_t;
  Series7() : Architecture("Series7") {}
  Series7(const std::string &name) : Architecture(name) {}
  static constexpr int words_per_frame = 101;
};

class UltraScale : public Series7 {
public:
  UltraScale() : Series7("UltraScale") {}
  using Part = xcuseries::Part;
  using FrameAddress = xcuseries::FrameAddress;
  static constexpr int words_per_frame = 123;
};

class UltraScalePlus : public Series7 {
public:
  UltraScalePlus() : Series7("UltraScalePlus") {}
  using Part = xcupseries::Part;
  using FrameAddress = xcupseries::FrameAddress;
  static constexpr int words_per_frame = 93;
};

class ArchitectureFactory {
public:
  static Architecture::Container create_architecture(const std::string &arch) {
    if (arch == "Spartan6") {
      return Spartan6();
    } else if (arch == "Series7") {
      return Series7();
    } else if (arch == "UltraScale") {
      return UltraScale();
    } else if (arch == "UltraScalePlus") {
      return UltraScalePlus();
    } else {
      throw absl::bad_variant_access();
    }
  }
};

} // namespace xilinx
} // namespace prjxray

#endif // PRJXRAY_LIB_XILINX_ARCHITECTURES_H_
