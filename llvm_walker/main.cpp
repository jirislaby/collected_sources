#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <cassert>
#include <fstream>

#include <llvm/Assembly/Parser.h>
#include <llvm/Module.h>
#include <llvm/Support/CFG.h>
#include <llvm/Support/GraphWriter.h>
#include <llvm/Function.h>
#include <llvm/GlobalVariable.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Argument.h>
#include <llvm/Constants.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instruction.h>
#include <llvm/Type.h>
#include <llvm/Value.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Bitcode/ReaderWriter.h>

//#include <conio.h>

void dumpType(llvm::Type const& type)
{
    std::string open,close;
    if (llvm::isa<llvm::StructType>(type))
    {open = "{"; close = "}";}
    else if (llvm::isa<llvm::ArrayType>(type))
    {open = ""; close = std::string("[") + "" + "]";}
    else if (llvm::isa<llvm::PointerType>(type))
    {open = ""; close = "*";}
    else
    {open = type.getDescription(); close = "";}

    std::cout << open;
    for (llvm::Type::subtype_iterator i = type.subtype_begin(),
				      e = type.subtype_end();
	  i != e; ++i)
	dumpType(**i);
    std::cout << close;
}

void dumpValue(llvm::Value const& value)
{
    if (value.hasName())
	std::cout << value.getName() << ": ";
    if (llvm::ConstantInt const* const CI =
	    llvm::dyn_cast<llvm::ConstantInt>(&value)) {
	std::cout << "\n";
	std::cout << "<CONST>= ";
	CI->dump();
	switch (CI->getBitWidth()) {
	case 8:
	    if (CI->getValue().isSignedIntN(8)) {
		std::cout << " <unsCh> ";
		char val = (char)CI->getSExtValue();
		switch (val) {
		case 0:
		    std::cout << "'\\0'";
		    break;
		case '\n':
		    std::cout << "'\\n'";
		    break;
		case '\t':
		    std::cout << "'\\t'";
		    break;
		default:
		    std::cout << '\'' << val << '\'';
		}
	    } else {
		std::cout << " <sigCh> ";
		unsigned char val = (unsigned char)CI->getZExtValue();
		switch (val) {
		case 0:
		    std::cout << "'\\0'";
		    break;
		case '\n':
		    std::cout << "'\\n'";
		    break;
		case '\t':
		    std::cout << "'\\t'";
		    break;
		default:
		    std::cout << '\'' << val << '\'';
		}
	    }
	    break;
	case 16:
	    if (CI->getValue().isSignedIntN(16))
	    {
		std::cout << " <unsSh> ";
		short val = (int)CI->getSExtValue();
		std::cout << val;
	    }
	    else
	    {
		std::cout << " <sigSh> ";
		unsigned short val = (unsigned short)CI->getZExtValue();
		std::cout << val;
	    }
	    break;
	case 32:
	    if (CI->getValue().isSignedIntN(32)) {
		std::cout << " <unsIn> ";
		int val = (int)CI->getSExtValue();
		std::cout << val;
	    } else {
		std::cout << " <sigIn> ";
		unsigned int val = (unsigned int)CI->getZExtValue();
		std::cout << val;
	    }
	    break;
	case 64:
	    if (CI->getValue().isSignedIntN(64)) {
		std::cout << " <unsLn> ";
		int val = (int)CI->getSExtValue();
		std::cout << val;
	    } else {
		std::cout << " <sigLn> ";
		unsigned long long val = (unsigned long long)CI->getZExtValue();
		std::cout << val;
	    }
	    break;
	default:
	    std::cout << "i??";
	}
	std::cout << " : ";
    }
    dumpType(*value.getType());
}

void dumpConstant(llvm::Constant const& constant)
{
    for (llvm::Constant::const_op_iterator i = constant.op_begin(),
					   e = constant.op_end();
	  i != e; ++i) {
	std::cout << "";
	dumpValue(**i);
	std::cout << '\n';
    }
}

void dumpGlobalVariable(llvm::GlobalVariable const& var)
{
    std::cout << "";
    dumpType(*var.getType()->getContainedType(0));
    std::cout << ' ' << var.getName();
    if (var.hasInitializer())
    {
	std::cout << " = {\n";
	dumpConstant(*var.getInitializer());
	std::cout << '}';
    }
    std::cout << ";\n";
}

void dumpInstruction(llvm::Instruction const& instr)
{
    std::cout << instr.getOpcodeName();
    for (unsigned int i = 0, E = instr.getNumOperands(); i != E; ++i)
    {
	if (i > 0) std::cout << ',';
	std::cout << ' ';
	dumpValue(*instr.getOperand(i));
    }
    if (instr.isTerminator())
	std::cout << " #";
    std::cout << '\n';
}

void dumpBasicBlock(llvm::BasicBlock const& block)
{
    std::cout << "basic_block(" << block.getName() << ") {\n";
    for (llvm::BasicBlock::const_iterator i = block.begin(),
					  e = block.end();
	  i != e; ++i)
	dumpInstruction(*i);
    std::cout << "}\n";
}

namespace llvm {
    template <>
    struct DOTGraphTraits<llvm::Function const*> : public DefaultDOTGraphTraits
    {
	static std::string
	getNodeLabel(BasicBlock const* const block, Function const* const)
	{
	    return block->getName();
	}
    };
}
void dumpCFG(llvm::Function const& function)
{
    std::string const cfgname = "cfg" + function.getName();
    std::ofstream fdot((cfgname + ".dot").c_str());

    if (0)
    {
	std::vector<llvm::BasicBlock const*> visited;
	std::queue<llvm::BasicBlock const*> toVisit;
	toVisit.push(&function.getEntryBlock());
	do
	{
	    llvm::BasicBlock const* const curr_block = toVisit.front();
	    toVisit.pop();
	    if (std::find(visited.begin(),visited.end(),curr_block)!=visited.end())
		continue;
	    visited.push_back(curr_block);

	    if (curr_block->hasName())
		fdot << curr_block->getName() << '\n';

	    for (llvm::succ_const_iterator i = llvm::succ_begin(curr_block),
					   e = llvm::succ_end(curr_block);
		i != e; ++i)
	    toVisit.push(*i);
	}
	while(!toVisit.empty());
    }
    else
	llvm::WriteGraph<llvm::Function const*>(fdot,&function,cfgname.c_str(),
						function.getName());
}

void dumpFunction(llvm::Function const& function)
{
    dumpType(*function.getReturnType());
    std::cout << ' ' << function.getName() << '(';
    std::string comma;
    for (llvm::Function::const_arg_iterator i = function.arg_begin(),
					    e = function.arg_end();
	  i != e; ++i) {
	std::cout << comma;
	dumpValue(*i);
	if (comma.empty())
	    comma = ", ";
    }
    std::cout << ')';
    if (function.isDeclaration())
    {
	std::cout << ";\n";
	return;
    }
    std::cout << " {\n";
    for (llvm::Function::const_iterator i = function.begin(),
					e = function.end();
	  i != e; ++i)
	dumpBasicBlock(*i);
    std::cout << "}\n";

    dumpCFG(function);
}

void dumpModule(llvm::Module const& module)
{
    for (llvm::Module::const_global_iterator i = module.global_begin(),
					     e = module.global_end();
	    i != e; ++i)
	dumpGlobalVariable(*i);
    for (llvm::Module::const_iterator i = module.begin(), e = module.end();
	    i != e; ++i)
	dumpFunction(*i);
}


int main(int argc, char *argv[])
{
	llvm::ParseError error;
	std::string err;
	std::auto_ptr<llvm::MemoryBuffer> membuf(llvm::MemoryBuffer::getFile("a.llo", &err));
	if (membuf.get() == NULL) {
		std::cerr << err << "\n";
		exit(1);
	}
	std::auto_ptr<llvm::Module> module(llvm::ParseBitcodeFile(membuf.get()));
//	    llvm::ParseAssemblyFile("a.ll",error));
	if (!module.get()) {
	    std::cout << "error:\n";
	    std::cout << error.getRawMessage() << "\n";
	    return -1;
	}

	dumpModule(*module);
	std::cout << "\n\n\nDone.\n";
	return 0;
}
