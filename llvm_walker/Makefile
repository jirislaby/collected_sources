LLVM_CXXFLAGS=$(shell llvm-config --cxxflags)
LLVM_LDFLAGS=$(shell llvm-config --ldflags --libs core asmparser bitreader)
CXXFLAGS=$(LLVM_CXXFLAGS) -g
LDFLAGS=$(LLVM_LDFLAGS)

all: walk a.llo a.ll

walk: main.o
	$(CXX) -o $@ $^ $(LDFLAGS)

%.llo: %.c
	llvm-gcc --emit-llvm -c -o $@ $^

%.ll: %.llo
	llvm-dis -f -o $@ $^

.PHONY: all
