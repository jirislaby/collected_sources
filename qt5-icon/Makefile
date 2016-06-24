QT_COMPS=Qt5Core Qt5Widgets Qt5Gui
CXXFLAGS=-fPIC -O2 -Wall $(shell pkg-config --cflags $(QT_COMPS)) 
LDFLAGS=-fPIC
LDLIBS=$(shell pkg-config --libs $(QT_COMPS)) 

all: tray

tray: main.o main.moc.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.moc.cpp: %.h
	moc-qt5 -o $@ $<
