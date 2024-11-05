#ifndef BOLT_PASSES_AddSanitizerpass_H
#define BOLT_PASSES_AddSanitizerpass_H

#include "bolt/Passes/BinaryPasses.h"

namespace llvm {
namespace bolt {

class AddressSanitizer : public BinaryFunctionPass {
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