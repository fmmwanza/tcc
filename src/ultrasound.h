#pragma once

#include <fstream>

#include <Cg/cg.h>
#include <Cg/cgGL.h>


#include <assert.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>


using namespace std;

#define ijn(a,b,n) ((a)*(n))+b

class Ultrasound
{
public:
	Ultrasound();
	Ultrasound(int width, int height, int depth, char *filename);
	 ~Ultrasound(void);

	void ultrasound();
	void getSlice();
	void applyTexture(unsigned char *pVolume);
	void display();
	void loadTexture(char *filename);
	void keyEvent(char key);
	void setScreen();
	void updateFromBratrack(int x);
    unsigned char ***volumeMatrix3D;
	GLuint textura;
	float p1X,p1Y,p1Z,p2X,p2Y,p2Z,p3X,p3Y,p3Z;
	int widthIn;
	int heightIn;
	int depthIn;
	unsigned char **datasetRaw;
	unsigned char **volumeMatrix2D;// = new unsigned char*[32];
	long mat_plano[32][32][32];
	int secondScreen;
};
