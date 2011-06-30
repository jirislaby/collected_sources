JAVA=java
JAVAC=javac
ANTLRFLAGS=-cp antlr-runtime-3.0.1.jar:antlr-2.7.7.jar:antlr-3.0.1.jar:stringtemplate-3.1b1.jar org.antlr.Tool
JAVACFLAGS=-cp antlr-runtime-3.0.1.jar:antlr-2.7.7.jar:antlr-3.0.1.jar:stringtemplate-3.1b1.jar
JAVAFLAGS=-cp .:antlr-runtime-3.0.1.jar
PROG=Test
SOURCE=T

all: $(PROG).class

run: all
	$(JAVA) $(JAVAFLAGS) $(PROG)

%.class: %.java
	$(JAVAC) $(JAVACFLAGS) $^

$(PROG).class: $(SOURCE)Lexer.java $(SOURCE)Parser.java $(SOURCE)Walker.java

$(SOURCE)Parser.java $(SOURCE)Lexer.java: $(SOURCE).g
	$(JAVA) $(ANTLRFLAGS) $<

$(SOURCE)Walker.java: $(SOURCE)Walker.g
	$(JAVA) $(ANTLRFLAGS) $<

#TParser.java: $(SOURCE)
#	$(JAVA) $(ANTLRFLAGS) $<

clean:
	rm -f *.class $(SOURCE)Parser.java $(SOURCE)Lexer.java \
		$(SOURCE)Walker.java \$(SOURCE)__.g $(SOURCE)*.tokens

.PHONY: compiler run all clean
