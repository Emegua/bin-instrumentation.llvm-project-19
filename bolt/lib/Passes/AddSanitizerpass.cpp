#include "bolt/Passes/AddSanitizerpass.h"
#include "bolt/Core/BinaryFunction.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace bolt;


Error AddressSanitizer::runOnFunctions(BinaryContext &BC) {
  outs() << "AddressSanitizer" << ": starting pass.\n";

  for (auto &BFI : BC.getBinaryFunctions()) {
    BinaryFunction &BF = BFI.second;
    if (BF.isSimple()) {
      outs() << ": found function " << BF.getPrintName() << "\n";
      // outs() << ADDRESS_SANITIZER_NAME << ": found function " << BF.getPrintName() << "\n";
    }
  }

  outs() << ": finished pass.\n";
  return Error::success();
}