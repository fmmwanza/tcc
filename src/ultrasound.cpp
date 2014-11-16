#include "ultrasound.h"
#include "mathutil/vector3f.h"
#include "medianfilter.h"

//#include <wavelet2s.h>

using namespace std;



Ultrasound::Ultrasound(){

}

Ultrasound::~Ultrasound(){

}


Ultrasound::Ultrasound(int width, int height, int depth, char *filename){

	widthIn = width;
	heightIn = height;
	depthIn = depth;
	p1X =0;// (int)widthIn/2;
	p2X = 0;//(int)widthIn/2;
	p3X = (int)widthIn/2;//(int)widthIn/2;

	p1Z = (int)widthIn/2;;
	p1Y = 0.0;

	p2Z = (int)widthIn/2;
	p2Y = (int)widthIn/2;

	p3Z = (int)widthIn/2;;
	p3Y = 0.0;
	windowWidth = 600;
	windowHeight = 600;

	glutInitWindowSize (windowWidth,windowHeight); 
	glutInitWindowPosition (900, 0);
	secondScreen = glutCreateWindow ("ultrasound");

	//glutSetWindow(mainScreen);
	loadTexture(filename);
}


void Ultrasound::applyTexture(unsigned char *pVolume){


	glGenTextures(1,&textura);
	glBindTexture( GL_TEXTURE_2D, textura);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,widthIn,heightIn, 0,GL_LUMINANCE, GL_UNSIGNED_SHORT,pVolume);
	
}

void Ultrasound::updateFromBratrack(int x){
			p1Z=x;p2Z=x;p3Z=x;
			getSlice();
}

void Ultrasound::getSlice(){

	int size = (widthIn + (int)(widthIn/2))*( heightIn + (int)(heightIn/2));
	unsigned char *pVolume = new unsigned char[size];
	//unsigned char *result = new unsigned char[size];
	int *tempArra = new int[size];
	const void* srcArr;

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
			pVolume[ijn(j,i,heightIn)] = datasetRaw[(int)interp1.z][ijn((int)interp1.y,(int)interp2.x,heightIn)];
			interp2 = interp2 + dx2 + v2;
		} 
		interp1 = interp1+dx1+v1;
		interp2 = p2;
	}

	//srcArr = (int)pVolume;

	// medianfilter(pVolume,result, widthIn, heightIn);	//Median filter
	//cvWiener2(pVolume,result,3,3);	//Wiener filter
	// medianfilter(result, pVolume, widthIn, heightIn);
	// medianfilter(pVolume,result, widthIn, heightIn);
	// medianfilter(result, pVolume, widthIn, heightIn);
	// medianfilter(pVolume,result, widthIn, heightIn);
	// medianfilter(result, pVolume, widthIn, heightIn);
	// medianfilter(pVolume,result, widthIn, heightIn);
	// medianfilter(result, pVolume, widthIn, heightIn);
	// medianfilter(pVolume,result, widthIn, heightIn);
	applyTexture(pVolume);

	delete [] pVolume;
	//delete [] result;
}

void Ultrasound::setScreen(){
	glutSetWindow(secondScreen);
}

void Ultrasound::display(){

	glutSetWindow(secondScreen);
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glViewport(0,0,600,600);        // Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);    // Select The Projection Matrix
	glLoadIdentity();               // Reset The Projection Matrix

	gluPerspective(20.0f,windowWidth/windowHeight,0.3f,100.0f);//(GLfloat)640/(GLfloat)480
	gluLookAt(0,0,10, 0,-2.5,0, 0,5,0);

    glEnable(GL_TEXTURE_2D);
   
    glBindTexture( GL_TEXTURE_2D, textura);

 //    glBegin(GL_QUADS);
	// 	glTexCoord2f(0.0, 0.0); glVertex2f(0.0, 0.0); 
	// 	glTexCoord2f(0.0, 0.5); glVertex2f(0.0, 1.5); 
	// 	glTexCoord2f(0.5, 0.5); glVertex2f(1.5, 1.5); 
	// 	glTexCoord2f(0.5, 0.0); glVertex2f(1.5, 0.0);
	// glEnd();

	const float PI = 3.14;
	const float MIN_ANGLE = -PI/8;
	const float MAX_ANGLE = PI/8;
	const float SECTOR_RAD = PI/2;

	//glEnable(GL_TEXTURE_2D);
    //glBindTexture( GL_TEXTURE_2D,  m_pTextureIds[1]);

	glBegin(GL_QUAD_STRIP);
	int x1 = 0;  //origin of sector array
	int y1 = 0;
	int xIni =0;

	float radius = 4.0;
	//float radius1 = 1.0;

	float t = 333.0;
	//Need to loop through A-lines and specify end then start for vertexes
	for(float angle = 3*PI/2 + MIN_ANGLE; angle <= (3*PI/2 + MAX_ANGLE + (SECTOR_RAD/t)); angle += SECTOR_RAD/t)
	{
		float vnorm = 1.5 - ((angle - PI - MIN_ANGLE)/SECTOR_RAD);
		glTexCoord2f(vnorm, 0.7);
		glVertex2f(x1,y1);   //beginning of A-line in image 
		glTexCoord2f(vnorm, 0.0); 
		glVertex2f(x1 + cos(angle)*radius, y1 + sin(angle)*radius);//end of A-line in image
		//xIni+=1.0;
	}
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

	//apply filter
	//Median filter
	for (int filterN = 0; filterN <= 5; ++filterN){
		for (int x = 0; x < widthIn; ++x){
			medianfilter(datasetRaw[x], datasetRaw[x], widthIn, heightIn);
		}
	}
	fclose(pFile);
}

void Ultrasound::keyEvent(char key){
	switch(key){

		case 'w':// left back
			//p1X+=0.5;p2X-=0.5;p3X-=0.5;
			getSlice(); 
			
		break;

		case 'z':// right back
			//p1X-=0.5;p2X+=0.5;p3X+=0.5;
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
		// case 's':// z face up
		// 	p1X-=0.5;p2X-=0.5;p3X-=0.5;
		// 	getSlice();
		// break;
		// case 'x':// z face back
		// 	p1X+=0.5;p2X+=0.5;p3X+=0.5;
		// 	getSlice();
		// break;
	}
	glutPostRedisplay();
}



