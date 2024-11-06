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

uint64_t AddressSanitizer::getAccessSize(const MCInst &Inst, const BinaryContext &BC) {
    const MCInstrDesc &Desc = BC.MII->get(Inst.getOpcode());
    
    // For x86, analyze instruction type
    StringRef Name = BC.MII->getName(Inst.getOpcode());
    
    // Common x86 memory access sizes
    if (Name.ends_with("b"))  return 1;  // byte
    if (Name.ends_with("w"))  return 2;  // word
    if (Name.ends_with("l"))  return 4;  // dword
    if (Name.ends_with("q"))  return 8;  // qword
    
    // For testing, print instruction info
    // outs() << "Analyzing size for instruction: " << Name << "\n";
    return 8; // Default to 8 bytes for now #FIXME
}

uint64_t AddressSanitizer::getAccessAddress(const MCInst &Inst, const BinaryContext &BC) {
    // Debug print operands
    // outs() << "Analyzing address for instruction with " 
    //        << Inst.getNumOperands() << " operands\n";

    // for (unsigned i = 0; i < Inst.getNumOperands(); ++i) {
    //     const MCOperand &Op = Inst.getOperand(i);
    //     if (Op.isReg()) {
    //         outs() << "  Op" << i << ": Register " << Op.getReg() << "\n";
    //     } else if (Op.isImm()) {
    //         outs() << "  Op" << i << ": Immediate " << Op.getImm() << "\n";
    //     }
    // }
    
    return 0xdeadbeef; // Placeholder
}

void AddressSanitizer::instrumentMemoryAccess(BinaryFunction &BF, MCInst &Inst) {
  NumInstrumentedInsts++;

  // Get instruction info
  const MCInstrDesc &Desc = BF.getBinaryContext().MII->get(Inst.getOpcode());
  StringRef InstName = BF.getBinaryContext().MII->getName(Inst.getOpcode());

  // Debug output - instruction type
  dbgs() << "Instruction: " << InstName << "  ";
  dbgs() << "Access type: ";
  if (Desc.mayLoad())
    dbgs() << "READ";
  if (Desc.mayStore())
    dbgs() << "WRITE";
  dbgs() << "\n";

  // Get memory access details
  uint64_t AccessSize = getAccessSize(Inst, BF.getBinaryContext());
  uint64_t Address = getAccessAddress(Inst, BF.getBinaryContext());
  
  // Calculate shadow memory address
  uint64_t ShadowAddr = (Address >> kShadowScale) + kShadowOffset;

  // Debug output - memory access details
  
  dbgs() << "Access size: " << AccessSize << " bytes\n";
  dbgs() << "Memory address: 0x" 
        << Twine::utohexstr(Address) << " ";
  dbgs() << "Shadow address: 0x" 
        << Twine::utohexstr(ShadowAddr) << "\n";

  // TODO: Insert actual instrumentation
  // insertShadowCheck(BF, Inst, ShadowAddr);
  
  // Print operands for debugging
  dbgs() << "Instruction details: ";
  dbgs() << "  Opcode: " << Inst.getOpcode() << " ";
  dbgs() << "  Operands: ";
  for (unsigned i = 0; i < Inst.getNumOperands(); ++i) {
    const MCOperand &Op = Inst.getOperand(i);
    dbgs() << "    Op" << i << ": ";
    if (Op.isReg())
      dbgs() << "Register " << Op.getReg();
    else if (Op.isImm())
      dbgs() << "Immediate " << Op.getImm();
    dbgs() << "   ";
  }
  dbgs() << "\n";
}

Error AddressSanitizer::runOnFunctions(BinaryContext &BC) {
  outs() << "AddressSanitizer" << ": starting pass.\n";

  for (auto &BFI : BC.getBinaryFunctions()) {
    BinaryFunction &BF = BFI.second;
    if (!BF.isSimple()) {
      dbgs() << "Found not a simple function " << BF.getPrintName() << "\n";
      // continue;
    }
    
    for (auto &BB : BF) {
      for (auto &Inst : BB) {
        if (isMemoryAccess(Inst, BC))
          instrumentMemoryAccess(BF, Inst);
      }
    }
  }

  // Print summary at end
  outs() << "AddressSanitizer: found " << NumInstrumentedInsts 
         << " memory instructions to instrument.\n";
  return Error::success();
}