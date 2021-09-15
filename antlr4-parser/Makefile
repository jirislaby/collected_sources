PROJ=par
GEN=$(PROJ)BaseListener.cpp $(PROJ)BaseListener.h $(PROJ)BaseVisitor.cpp $(PROJ)BaseVisitor.h $(PROJ)Lexer.cpp $(PROJ)Lexer.h $(PROJ)Listener.cpp $(PROJ)Listener.h $(PROJ)Parser.cpp $(PROJ)Parser.h $(PROJ)Visitor.cpp $(PROJ)Visitor.h
SRCS=main.cpp $(filter %.cpp,$(GEN))
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
CXXFLAGS=-I/usr/include/antlr4-runtime -Wno-attributes -Wall -g -ggdb -Og -fPIC -flto
LDFLAGS=-Og -fPIC -flto
LDLIBS=-lantlr4-runtime
ANTLR=/tmp/antlr-4.8-complete.jar

all: $(PROJ) FORCE
	./$< < in

%BaseListener.cpp %BaseListener.h %BaseVisitor.cpp %BaseVisitor.h %Lexer.cpp %Lexer.h %Listener.cpp %Listener.h %Parser.cpp %Parser.h %Visitor.cpp %Visitor.h: %.g4
	java -jar $(ANTLR) -visitor -Dlanguage=Cpp $^

main.cpp: $(filter %.h,$(GEN))

par: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f $(PROJ) $(GEN) *.o *.tokens *.interp

.PHONY: FORCE
