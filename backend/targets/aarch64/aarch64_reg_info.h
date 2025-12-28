#ifndef __BACKEND_TARGETS_AARCH64_AARCH64_REG_INFO_H__
#define __BACKEND_TARGETS_AARCH64_AARCH64_REG_INFO_H__

#include <backend/target/target_reg_info.h>
#include <backend/targets/aarch64/aarch64_defs.h>
#include <map>

namespace BE::Targeting::AArch64
{
    class RegInfo : public TargetRegInfo
    {
      public:
      RegInfo()
      {
        // 初始化各类寄存器集合
        intRegs_ = [] {
          std::vector<int> v;
          for (int i = 0; i <= BE::AArch64::A64_MAX_GPR_ID; ++i) v.push_back(i);
          return v;
        }();

        floatRegs_ = [] {
          std::vector<int> v;
          for (int i = 0; i <= BE::AArch64::A64_MAX_FPR_ID; ++i) v.push_back(i);
          return v;
        }();

        intArgRegs_   = {0, 1, 2, 3, 4, 5, 6, 7};          // x0-x7 / w0-w7
        floatArgRegs_ = {0, 1, 2, 3, 4, 5, 6, 7};           // d0-d7 / s0-s7

        calleeSavedIntRegs_   = {19, 20, 21, 22, 23, 24, 25, 26, 27, 28};  // x19-x28
        calleeSavedFloatRegs_ = {8, 9, 10, 11, 12, 13, 14, 15};            // d8-d15

        // 保留寄存器：sp/zero 以及 ABI 约定的特殊寄存器
        // - x29: fp
        // - x30: lr
        // - x18: 平台保留（AAPCS64 建议视平台而定；Linux 下常作为平台寄存器）
        // 另外 sp/xzr 也不可用于分配。
        reservedRegs_ = {
            // Argument / return registers (caller-saved). Keep them out of RA to avoid
            // clobbering ABI live-ins/live-outs (e.g. during param moves / call sequences).
            0, 1, 2, 3, 4, 5, 6, 7,
            29,
            30,
            18,
          16,
          17,
          15, // Reserve x15 as a dedicated scratch register for StackLowering (address calculation)
            BE::AArch64::A64_REGISTER_ID_SP,
            BE::AArch64::A64_REGISTER_ID_XZR,
        };
      }

      int spRegId() const override { return BE::AArch64::A64_REGISTER_ID_SP; }
      int raRegId() const override { return 30; }  // x30 / lr
      int zeroRegId() const override { return BE::AArch64::A64_REGISTER_ID_XZR; }

      const std::vector<int>& intArgRegs() const override { return intArgRegs_; }
      const std::vector<int>& floatArgRegs() const override { return floatArgRegs_; }

      const std::vector<int>& calleeSavedIntRegs() const override { return calleeSavedIntRegs_; }
      const std::vector<int>& calleeSavedFloatRegs() const override { return calleeSavedFloatRegs_; }

      const std::vector<int>& reservedRegs() const override { return reservedRegs_; }

      const std::vector<int>& intRegs() const override { return intRegs_; }
      const std::vector<int>& floatRegs() const override { return floatRegs_; }

      private:
      std::vector<int> intRegs_;
      std::vector<int> floatRegs_;
      std::vector<int> intArgRegs_;
      std::vector<int> floatArgRegs_;
      std::vector<int> calleeSavedIntRegs_;
      std::vector<int> calleeSavedFloatRegs_;
      std::vector<int> reservedRegs_;
    };
}  // namespace BE::Targeting::AArch64

#endif  // __BACKEND_TARGETS_AARCH64_AARCH64_REG_INFO_H__
