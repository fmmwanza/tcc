CC = g++
CFLAGS = -Wall 
CDEBUG = -g
COPTIMIZATION = -O4
WARNINGOFF = -w
SUPPORT = -std=c++11
PROG = ../bin/volumeRendering

SRC = main.cpp core.cpp GlutWindow.cpp medianfilter.cpp BraTrackUDP.cpp ultrasound.cpp ArcBall.cpp TransferFunction.cpp Trapez.cpp mathutil/CMatrix.cpp mathutil/CPosition.cpp mathutil/CQuaternion.cpp mathutil/CVector.cpp
OPENGL = -lglut -lGL -lGLU -lGL
OPENCV = `pkg-config opencv --libs --cflags`
CG = -lCg -lCgGL

$(PROG): $(SRC)
	 $(CC) $(CFLAGS) $(CDEBUG) $(WARNINGOFF) $(SUPPORT) -o $(PROG) $(SRC) $(OPENGL) $(CG)

clean:
	rm -f $(PROG)

run: 
	sh ../bin/runSingleTest.sh 


all:
	make clean
	make $(PROG)
	make run




