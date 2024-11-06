#ifndef BOLT_PASSES_AddSanitizerpass_H
#define BOLT_PASSES_AddSanitizerpass_H

#include "bolt/Passes/BinaryPasses.h"

namespace llvm {
namespace bolt {

class AddressSanitizer : public BinaryFunctionPass {
  private:
    // 8 bytes of app memory map to 1 shadow byte (2^3 = 8)
    const uint64_t kShadowScale = 3;  
    const uint64_t kShadowOffset = 0x7fff8000;
    uint64_t NumInstrumentedInsts = 0;

  private:
    // Mem access detection
    bool isMemoryAccess(const MCInst &Inst, const BinaryContext &BC);
    uint64_t getAccessSize(const MCInst &Inst, const BinaryContext &BC);
    uint64_t getAccessAddress(const MCInst &Inst, const BinaryContext &BC);


    // instrumentation logic
    void instrumentMemoryAccess(BinaryFunction &BF, MCInst &Inst);
    void insertShadowCheck(BinaryFunction &BF, MCInst &Inst, uint64_t ShadowAddr);
    
    // Runtime support
    void insertAsanInit(BinaryContext &BC);
    void insertErrorReporting(BinaryFunction &BF, MCInst &Inst);

  public:
    explicit AddressSanitizer(const cl::opt<bool> &PrintPass)
        : BinaryFunctionPass(PrintPass) {}

    /// getName - Return the pass name.
    const char *getName() const override { return "Memory Address Sanitizer"; }

    /// runOnFunctions - This method implements the pass logic.
    Error runOnFunctions(BinaryContext &BC) override;
};

} // namespace bolt
} // namespace llvm

#endif // BOLT_PASSES_AddSanitizerpass_H