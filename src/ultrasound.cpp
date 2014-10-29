#include "ultrasound.h"
#include "mathutil/vector3f.h"


using namespace std;

Ultrasound::Ultrasound(){

}

Ultrasound::~Ultrasound(){

}


Ultrasound::Ultrasound(int width, int height, int depth, char *filename){

	widthIn = width;
	heightIn = height;
	depthIn = depth;
	p1X = (int)widthIn/2;
	p2X = (int)widthIn/2;
	p3X = (int)widthIn/2;

	p1Z = 0.0;
	p1Y = 0.0;

	p2Z = 0.0;
	p2Y = widthIn;

	p3Z = widthIn;
	p3Y = 0.0;

	glutInitWindowSize (600,400); 
	glutInitWindowPosition (900, 0);
	secondScreen = glutCreateWindow ("ultrasound");

	//glutSetWindow(mainScreen);
	loadTexture(filename);
}


void Ultrasound::applyTexture(unsigned char *pVolume){


	glGenTextures(1,&textura);
	glBindTexture( GL_TEXTURE_2D, textura);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,widthIn,heightIn, 0,GL_LUMINANCE, GL_UNSIGNED_SHORT,pVolume);
	
}

void Ultrasound::getSlice(){

	int size = widthIn*heightIn;
	unsigned char *pVolume = new unsigned char[size];

    vector3f p1(p1X,p1Y,p1Z);
	vector3f p2(p2X,p2Y,p2Z);
	vector3f p3(p3X,p3Y,p3Z);

	vector3f v1 = sub(p2,p1);
	vector3f v2 = sub(p3,p1);
	vector3f dx1 = v1/5000;
	vector3f dx2 = v2/5000;

	v1.normalize();
	v2.normalize();

	vector3f interp1 = p1;
	vector3f interp2 = p2;

	for (int i = 0; i < widthIn; ++i){
		for (int j = 0; j < heightIn; ++j){
			//pVolume[ijn(i,j,heightIn)] = datasetRaw[(int)interp1.z][ijn((int)interp1.y,(int)interp2.x,heightIn)];
			pVolume[ijn(j,i,heightIn)] = datasetRaw[(int)interp1.x][ijn((int)interp1.y,(int)interp2.z,heightIn)];
			interp2 = interp2 + dx2 + v2;
		} 
		interp1 = interp1+dx1+v1;
		interp2 = p2;
	}
				
	applyTexture(pVolume);

	delete [] pVolume;
}

void Ultrasound::setScreen(){
	glutSetWindow(secondScreen);
}

void Ultrasound::display(){

	glutSetWindow(secondScreen);
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glViewport(0,0,640,480);        // Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);    // Select The Projection Matrix
	glLoadIdentity();               // Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(20.0f,1.0,0.3f,200.0f);//(GLfloat)640/(GLfloat)480
	gluLookAt(0,0,5, 0.5,0.5,0, 0,1,0);

    glEnable(GL_TEXTURE_2D);
   
    glBindTexture( GL_TEXTURE_2D, textura);

    glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2f(0.0, 0.0); 
		glTexCoord2f(0.0, 0.5); glVertex2f(0.0, 1.0); 
		glTexCoord2f(0.5, 0.5); glVertex2f(1.0, 1.0); 
		glTexCoord2f(0.5, 0.0); glVertex2f(1.0, 0.0);
	glEnd();
	glFlush();
 	glutSwapBuffers();
}


void Ultrasound::loadTexture(char *filename){

		//open input dataset file
	FILE *pFile = fopen(filename,"rb");
	if (NULL == pFile) {
		printf(" Inpunt file doesn't exist\n");
		exit(0);
	}

	// allocate memory for the 3d dataset
	datasetRaw = (unsigned char**)malloc(depthIn * sizeof(unsigned char*));
	for (int i=0; i < widthIn; i++)
		datasetRaw[i] = (unsigned char*)malloc(sizeof(unsigned char) * (widthIn*heightIn));

	for( int d = 0; d < depthIn; d++ )
		for( int w = 0; w < widthIn; w++ )
			for( int h = 0; h < heightIn; h++ )
			{
				unsigned char value;
				fread( &value, 1, sizeof(unsigned char), pFile);
				//datasetRaw[d][w * heightIn + h] = value;
				datasetRaw[h][ijn(w,d,heightIn)] = value;
			}

	fclose(pFile);
}

void Ultrasound::keyEvent(char key){
	switch(key){

		case 'w':// left back
			p1X+=0.5;p2X-=0.5;p3X-=0.5;
			getSlice(); 
			
		break;

		case 'z':// right back
			p1X-=0.5;p2X+=0.5;p3X+=0.5;
			getSlice();
		break;

		case 'a':// up back
			/*p1X-=0.5;*/p2X-=0.2;p3X+=0.5;
			getSlice();
		break;
		case 'd':// bottom back
			/*p1X+=0.5;*/p2X+=0.2;p3X-=0.5;
			getSlice();
		break;
		case 's':// z face up
			p1X-=0.5;p2X-=0.5;p3X-=0.5;
			getSlice();
		break;
		case 'x':// z face back
			p1X+=0.5;p2X+=0.5;p3X+=0.5;
			getSlice();
		break;
	}
	glutPostRedisplay();
}