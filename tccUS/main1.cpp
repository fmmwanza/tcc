//g++ -w -Wall -std=c++11 main.cpp BraTrackUDP.cpp -o  test -lGL -lglut -lGLU


#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>

#include "vector3f.h"
#include "BraTrackUDP.h"
//#include "plane.h"

using namespace std;
using namespace BraTrackUDP;

typedef unsigned char volume;

#define PI 3.141592
#define ijn(a,b,n) ((a)*(n))+b

unsigned char ***volumeMatrix3D;
int fpsCount ;
int fpsCurrent;
int left,right,middle;
GLuint textura;
int u, i, o;
float v_plano [] = {0.0,0.0,1.0,0.0};
float dEquation, xx,yy,zz;
//float cosA, cosB, cosG;


int firstIgnore = 0;
int rotxUltrasound = 0;
bool transducerOut = false;
float transducerX = 0;
float initialPosBratrack;
unsigned short int SERVICE_PORT = 3000; 	//port service for udp connection
unsigned int BUFFER_SIZE = 1024;


int p1X =0;// (int)widthIn/2;
int p2X = 0;//(int)widthIn/2;
int p3X = (int)(256/2);//(int)widthIn/2;

int p1Z = (int)(256/2);
int p1Y = 0.0;

int p2Z = (int)(256/2);
int p2Y = (int)(256/2);

int p3Z = (int)(256/2);
int p3Y = 0.0;
float nx = 1;
float ny = 0;
float nz = 1;


// float cosA = normalVect.x / sqrt(pow(normalVect.x,2) + pow(normalVect.y,2) + pow(normalVect.z,2));
// float cosB = normalVect.y / sqrt(pow(normalVect.x,2) + pow(normalVect.y,2) + pow(normalVect.z,2));
// float cosG = normalVect.z / sqrt(pow(normalVect.x,2) + pow(normalVect.y,2) + pow(normalVect.z,2));
// float step = 1;

BraTrackUDP::classInputProtocol udpReceiver;
vector<type_artifact> list_of_artifacts;


unsigned char **datasetRaw;

void countFPS(int value) {
	char title[120];
	fpsCount = fpsCurrent;
	fpsCurrent = 0;
	
	snprintf(title, 120, "%s. FPS: %d", "Image Match", fpsCount);
	glutSetWindowTitle(title);
	glutTimerFunc(1000, countFPS, 1);
}

float planeEquation(int x, int y, int z){

	vector3f p1(p1Z,p1Z,p1Z);
	vector3f normalVect(nx,ny,nz);

	//float p11 = p1.x;
	// p1.x = (p1.x +  (step*cosA));
	// p1.y = (p1.y +  (step*cosB));
	// p1.z = (p1.z +  (step*cosG));

	//p1.set((p1.x +  (step*cosA)), (p1.y + (step*cosB)), (p1.z + (step*cosG)));

	//printf("p1Antes: %f - cosG: %f -- step:  %f -- magia: %f \n", p11, cosG, step,  p1.y );



	float d = -(p1.x*normalVect.x + p1.y*normalVect.y + p1.z*normalVect.z);

	//cout << d << endl;

	return normalVect.x*x + normalVect.y*y + normalVect.z*z + d;

}


void vectToMat3(unsigned char* pVolume){

	//Allocate memory to 3d matrix
	volumeMatrix3D = new unsigned char**[256];

	for(int i = 0; i < 256; i++)
        volumeMatrix3D[i] = new unsigned char*[256];
	
	for(int i = 0; i < 256; i++)
        for(int j = 0; j < 256; j++){
            volumeMatrix3D[i][j] = new unsigned char[256];
        }
    

    //Transfer data - Array to Matrix
	for(int x = 0; x < 256;x++)
		for (int y = 0; y < 256; y++)
			for (int z = 0; z < 256; z++){
				volumeMatrix3D[z][y][x] = pVolume[z * 256 * 256 + y * 256 + x];
				
				//voxel(x,y,z) = array[z * YDIM * XDIM + y * XDIM + x],
			}
}

void applyTexture(unsigned char *pVolume){


	glGenTextures(1,&textura);
	glBindTexture( GL_TEXTURE_2D, textura);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,256,256, 0,GL_LUMINANCE, GL_UNSIGNED_SHORT,pVolume);	
}

void _medianfilter(const volume* image, volume* result, int N, int M)
{
   //   Move window through all elements of the image
   for (int m = 1; m < M - 1; ++m)
      for (int n = 1; n < N - 1; ++n)
      {
         //   Pick up window elements
         int k = 0;
         volume window[9];
         for (int j = m - 1; j < m + 2; ++j)
            for (int i = n - 1; i < n + 2; ++i)
               window[k++] = image[j * N + i];
         //   Order elements (only half of them)
         for (int j = 0; j < 5; ++j)
         {
            //   Find position of minimum element
            int min = j;
            for (int l = j + 1; l < 9; ++l)
            if (window[l] < window[min])
               min = l;
            //   Put found minimum element in its place
            const volume temp = window[j];
            window[j] = window[min];
            window[min] = temp;
         }
         //   Get result - the middle element
         result[(m - 1) * (N - 2) + n - 1] = window[4];
      }
}

//   2D MEDIAN FILTER wrapper
//     image  - input image
//     result - output image
//     N      - width of the image
//     M      - height of the image
void medianfilter(volume* image, volume* result, int N, int M)
{
   //   Check arguments
   if (!image || N < 1 || M < 1)
      return;
   //   Allocate memory for signal extension
   volume* extension = new volume[(N + 2) * (M + 2)];
   //   Check memory allocation
   if (!extension)
      return;
   //   Create image extension
   for (int i = 0; i < M; ++i)
   {
      memcpy(extension + (N + 2) * (i + 1) + 1, image + N * i, N * sizeof(volume));
      extension[(N + 2) * (i + 1)] = image[N * i];
      extension[(N + 2) * (i + 2) - 1] = image[N * (i + 1) - 1];
   }
   //   Fill first line of image extension
   memcpy(extension, extension + N + 2, (N + 2) * sizeof(volume));
   //   Fill last line of image extension
   memcpy(extension + (N + 2) * (M + 1), extension + (N + 2) * M, (N + 2) * sizeof(volume));
   //   Call median filter implementation
   _medianfilter(extension, result ? result : image, N + 2, M + 2);
   //   Free memory
   delete[] extension;
}

unsigned char *ultrasoundSpeckle(unsigned char *pVolume){

	float maxValue = 2*PI;
	std::default_random_engine generator;
  	std::uniform_real_distribution<double> distribution(0,maxValue);
  	int sizeVect = (256+128)*(256+128);
	unsigned char *speckle = new unsigned char[sizeVect];

	for (int i=0; i<sizeVect; ++i) {
   	 double number = distribution(generator);
   	 speckle[i] = pVolume[i] + number*pVolume[i];
  	}
	
	medianfilter(speckle, speckle, 256, 256);
	return speckle;
}

void getSlice(){

	int w,v;
	unsigned char *pVolume = new unsigned char[(256+128)*(256+128)];


    int counter =0;

 //    vector3f p1(p1X,p1Y ,p1Z);
	// vector3f p2(p2X,p2Y,p2Z);
	// vector3f p3(p3X,p3Y,p3Z);

	// vector3f v1 = sub(p2,p1);
	// vector3f v2 = sub(p3,p1);
	// vector3f dx1 = v1/3000;
	// vector3f dx2 = v2/3000;

	// v1.normalize();
	// v2.normalize();

	// vector3f interp1 = p1;
	// vector3f interp2 = p2;

	// for (int i = 0; i < 256; ++i){
	// 	for (int j = 0; j < 256; ++j){
	// 		pVolume[ijn(j,i,256)] = datasetRaw[(int)interp1.z][ijn((int)interp1.y,(int)interp2.x,256)];
	// 		interp2 = interp2 + dx2 + v2;
	// 		//cout << interp1.z <<endl;
	// 	} 
	// 	interp1 = interp1+dx1+v1;
	// 	interp2 = p2;
	// }
	 int i =0;
	for (int x = 0; x < 256; ++x)
	for (int y = 0; y < 256; y++){
 		for (int z = 0; z < 256; z++){
 			//float zero = x*plane.x + y*plane.y + z*plane.z + dEquation;
 			float zero = planeEquation(x,y,z);
 			if((zero < 0.001) && (zero > -0.001))//(v_plano[3]*z + v_plano[2]*y + v_plano[1]*x + v_plano[0] < 0.00001)	//This is how I find my plane intersection, I'm not sure :(
 			{
 				//volumeMatrix2D[x][y] = datasetRaw[x][ijn(y,z,256)];//
 				//volumeMatrix2D[x][y] = 
 				//volume1rix2D[x][y] = volumeMatrix3D[x][y][z];
 				//printf("%i\n", counter );
 				i++;
 				
 				pVolume[counter] = volumeMatrix3D[x][y][z];
 				//pVolume[z * 256 * 256 + y * 256 + x] = volumeMatrix3D[x][y][z];
 				counter++;
 			} 
 		}
 	}

 	cout << i<<endl;

	applyTexture(pVolume);
	delete [] pVolume;
}


// void getPlane(){

// 	float alfa, beta, gama;
// 	float angleOP = 
// 	vector3f point(127,127 ,127);
// 	vector3f normal(1,0,1);


// }


void loadTexture(){

		//open input dataset file
	FILE *pFile = fopen("skull.raw","rb");
	if (NULL == pFile) {
		printf(" Input file doesn't exist\n");
		exit(0);
	}

	int size = 256*256*256;

	unsigned char *pVolume = new unsigned char[size];
	if(size == fread(pVolume,sizeof(unsigned char), size,pFile));
	vectToMat3(pVolume);



	// datasetRaw = (unsigned char**)malloc(256 * sizeof(unsigned char*));
	// for (int i=0; i < 256; i++)
	// 	datasetRaw[i] = (unsigned char*)malloc(sizeof(unsigned char) * (256*256));

	// for( int d = 0; d < 256; d++ )
	// 	for( int w = 0; w < 256; w++ )
	// 		for( int h = 0; h < 256; h++ )
	// 		{
	// 			unsigned char value;
	// 			fread( &value, 1, sizeof(unsigned char), pFile);
	// 			datasetRaw[h][ijn(w,d,256)] = value;
	// 		}
	//----------------------

	// First, add speckle noise then
	// Apply Median filter

	// GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_24;
	// for (int i = 0;i < 256 ; ++i){
	// 		int percent =0;
	// 		datasetRaw[i] = ultrasoundSpeckle(datasetRaw[i]);
	// 		//percent = (i * 100)/heightIn;
	// 		cout << "Filtering :" << i << "/256 Slices"<<endl;
	// 	}

	fclose(pFile);

	getSlice();
}


void reshape(int w, int h){
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
}




void idle(){

	unsigned long long int time_stamp;
	// try{
	// 	udpReceiver.receive_frame_not_blocking(time_stamp,list_of_artifacts);
	// }
	// catch(...){
	// 	cout << "error receiver" << std::endl;
	// }
	glutPostRedisplay();
}

void init(){
	u=15;
	i=15;
	//p1X = 128;
	//p2X = 128;
	//p3X = 128;
	fpsCount = 0;
	fpsCurrent =0;

	if(udpReceiver.init(SERVICE_PORT,BUFFER_SIZE))
		cout<<"Error UDP connection"<<endl;

	udpReceiver.set_package_limits(4); //maximum number of artifacts
	type_artifact art1;
	art1.id = "1";
	list_of_artifacts.push_back(art1);
	initialPosBratrack =0;

	loadTexture();
	glClearColor(0.0,0.0,0.0,0.0);

}

void drawTransducer(){

	firstIgnore++;
	double transform[16];
	int i=0;
		type_artifact & temp = list_of_artifacts[i];
		transform[0] = temp.transform[0];
		transform[1] = temp.transform[1];
		transform[2] = temp.transform[2];
		transform[3] = 0.0;
		transform[4] = temp.transform[3];
		transform[5] = temp.transform[4];
		transform[6] = temp.transform[5];
		transform[7] = 0.0;
		transform[8] = temp.transform[6];
		transform[9] = temp.transform[7];
		transform[10] = temp.transform[8];
		transform[11] = 0.0;
		// translation x
		transform[12] = temp.transform[9]*0.005;
		transform[13] = 0;//temp.transform[10];
		transform[14] = 0;//temp.transform[11];
		transform[15] = 1.0;

		//cout << transform[i] <<endl;

		if(firstIgnore > 1){
			transducerX = (int)((255/2) + transform[12]*130);
			
			if((transducerX < 255) && (transducerX > 0)){
				rotxUltrasound = transducerX;
				transducerOut = false;
			}else{						// transducer out of target
				transducerOut = true;
			if(transducerX > 255)
				rotxUltrasound = 255 - 1;
			else
				rotxUltrasound = 1;
			}		
		}	

		firstIgnore++;
		//p1Z=rotxUltrasound;p2Z=rotxUltrasound;p3Z=rotxUltrasound;
}

void display(){

	getSlice();
	float x,y,xcos,ysin,tx,ty;
	// glClearColor(1.0,1.0,1.0,0.0);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// glViewport(0,0,640,480);        // Reset The Current Viewport

	// glMatrixMode(GL_PROJECTION);    // Select The Projection Matrix
	// glLoadIdentity();               // Reset The Projection Matrix

	// // Calculate The Aspect Ratio Of The Window
	// gluPerspective(15.0f,1.0,0.3f,200.0f);//(GLfloat)640/(GLfloat)480
	// gluLookAt(0,0,15, 0,-3,0, 0,1,0);
	// //gluLookAt(0,0,5, 0.5,0.5,0, 0,1,0);

 //    glEnable(GL_TEXTURE_2D);




    glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
    
	glViewport(0,0,600,600);        // Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);    // Select The Projection Matrix
	glLoadIdentity();               // Reset The Projection Matrix

	gluPerspective(20.0f,600/600,0.3f,100.0f);//(GLfloat)640/(GLfloat)480
	gluLookAt(0,0,10, 0,-2.5,0, 0,5,0);

    glEnable(GL_TEXTURE_2D);



	const float MIN_ANGLE = -PI/8;
	const float MAX_ANGLE = PI/8;
	const float SECTOR_RAD = PI/2;

	glBegin(GL_QUAD_STRIP);
	int x1 = 0;  //origin of sector array
	int y1 = 0;
	int xIni =0;

	float radius = 4.0;
	//float radius1 = 1.0;

	float t = 333.0;
	//Need to loop through A-lines and specify end then start for vertexes
	//int i=0;
	for(float angle = 3*PI/2 + MIN_ANGLE; angle <= (3*PI/2 + MAX_ANGLE + (SECTOR_RAD/t)); angle += SECTOR_RAD/t)
	{
		float vnorm = 1.5 - ((angle - PI - MIN_ANGLE)/SECTOR_RAD);
		glTexCoord2f(vnorm, 0.7);
		glVertex2f(x1,y1);   //beginning of A-line in image 
		glTexCoord2f(vnorm, 0.0); 
		glVertex2f(x1 + cos(angle)*radius, y1 + sin(angle)*radius);//end of A-line in image
		//i++;
	}
	//cout << i <<endl;
	glEnd();

	drawTransducer();

	glutSwapBuffers();
	glFlush();
}

void teclado(unsigned char tecla, int x, int y){
		switch(tecla){

		case 'w':// left back
			//p1Z+=1;//p2Z-=1;p3Z-=1;
			//display(); 
			nx+=0.0001;
			
		break;

		case 'z':// right back
			//p1Z-=1;//p2Z+=1;p3Z+=1;
			//display();
			nx-=0.0001;
		break;

		case 'a':// up back
			//p1X-=1;p2Z-=1;p3Z+=1;
			//display();
			ny -=0.0001;
		break;
		case 'd':// bottom back
			//p1X+=1;p2Z+=1;p3Z-=1;
			//display();
			ny+=0.0001;
		break;
		case 's':// z face up
			//p1Z-=1;p2Z-=1;p3Z-=1;
			//display();
			nz+=0.0001;
		break;
		case 'x':// z face back
			//p1Z+=1;p2Z+=1;p3Z+=1;
			//display();
			nz-=0.0001;
		break;
		case 'q':// z face back
			//nz-=0.1;
			exit(1);
		break;
		default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Ultrasound");
	glutKeyboardFunc(teclado);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
	return 0;
}


