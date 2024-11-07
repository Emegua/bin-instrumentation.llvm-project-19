#include "bolt/Passes/AddSanitizerpass.h"
#include "bolt/Core/BinaryFunction.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace bolt;

bool AddressSanitizer::isMemoryAccess(const MCInst &Inst, const BinaryContext &BC) {
  const MCInstrDesc &Desc = BC.MII->get(Inst.getOpcode());
  return Desc.mayLoad() || Desc.mayStore();
}

bool isUserFunction(const BinaryFunction &BF) {
  // Example logic based on function name
  StringRef Name = BF.getPrintName();
  // Skip functions starting with "__" (common for system/library functions)
  if (Name.starts_with("__"))
    return false;
  // Skip other known non-user functions
  if (Name == "init" || Name == "fini")
    return false;
  return true;
}

Error AddressSanitizer::runOnFunctions(BinaryContext &BC) {
  outs() << "AddressSanitizer: starting pass.\n";

  for (auto &BFI : BC.getBinaryFunctions()) {
    BinaryFunction &BF = BFI.second;

    if (!isUserFunction(BF) && BF.getPrintName() != "main") {
      dbgs() << "Skipping non-user function: " << BF.getPrintName() << "\n";
      continue;
    }
  
    dbgs() << "Instrumenting function: " << BF.getPrintName() << "\n";
    for (auto &BB : BF) {
      for (auto &Inst : BB) {
        if (isMemoryAccess(Inst, BC))
          // instrumentMemoryAccess(BF, Inst);
          if (!BC.MIB->instrumentMemoryAccess(Inst, *BC.STI))
          {
            dbgs() << "Failed to instrument instruction in " << BF.getPrintName() << "\n";
          }
          NumInstrumentedInsts++;
      }
    }
  }

  outs() << "AddressSanitizer: found " << NumInstrumentedInsts 
         << " memory instructions to instrument.\n";
  return Error::success();
}
