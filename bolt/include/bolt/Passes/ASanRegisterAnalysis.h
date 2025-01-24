// ASanRegisterAnalysis.h
#ifndef BOLT_PASSES_ASANREGISTERANALYSIS_H
#define BOLT_PASSES_ASANREGISTERANALYSIS_H

#include "bolt/Core/BinaryFunction.h"
#include "bolt/Passes/LivenessAnalysis.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/ADT/BitVector.h"

namespace llvm {
namespace bolt {

class ASanRegisterAnalysis {
private:
  BinaryFunction &BF;
  const MCRegisterInfo &MRI;
  LivenessAnalysis &LA;
  BitVector ReservedRegs;

public:
  ASanRegisterAnalysis(BinaryFunction &BF, LivenessAnalysis &LA)
    : BF(BF),
      MRI(*BF.getBinaryContext().MRI),
      LA(LA) {
    ReservedRegs.resize(MRI.getNumRegs());
    reserveShadowRegisters();
  }

  // Core functionality
  MCPhysReg findFreeRegisterAt(const MCInst &Inst);
  bool isRegisterAvailable(MCPhysReg Reg, const MCInst &Inst);

private:
  void reserveShadowRegisters();
  bool isCalleeSaved(MCPhysReg Reg) const;
  MCPhysReg findBestCandidate(const BitVector &LiveRegs);
};

} // namespace bolt
} // namespace llvm
#endif