#include "core.h"
#include "ultrasound.h"

CGlutWindow *Core::c_pMainWindow=0;
Ultrasound *secondWindow;
unsigned int Core::fpsCurrent=0;
unsigned int Core::fpsCount=0;
bool setUltrasound;
DATAINFO c_datasetInfo;


Core::Core(int c_argc, char **c_argv)
{

	glutInit(&c_argc, c_argv);

	c_datasetInfo.inputFileName= c_argv[1];
	c_datasetInfo.resWidth	   = atoi(c_argv[2]);
	c_datasetInfo.resHeight	   = atoi(c_argv[3]);
	c_datasetInfo.initStack    = atoi(c_argv[4]);
	c_datasetInfo.endStack     = atoi(c_argv[5]);
	c_datasetInfo.resDepth     = c_datasetInfo.endStack - c_datasetInfo.initStack;
	c_pMainWindow = new CGlutWindow(c_datasetInfo);	

	// if(toupper(c_argv[6][0]) == 'Y')
	// {
	// 	secondWindow = new Ultrasound(c_datasetInfo.resWidth,c_datasetInfo.resHeight,c_datasetInfo.resDepth, c_datasetInfo.inputFileName);
	// 	//c_pMainWindow->setScreen();
	// 	setUltrasound = true;
	// }
}

Core::~Core(){}


void Core::run()
{
	glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(move);
    glutTimerFunc(1000, countFPS, 1);
	//glutIdleFunc(idle);

	secondWindow = new Ultrasound(c_datasetInfo.resWidth,c_datasetInfo.resHeight,c_datasetInfo.resDepth, c_datasetInfo.inputFileName);
	glutDisplayFunc(displayUS);
	glutReshapeFunc(reshapeUS);
	glutIdleFunc(idle);

	glutMainLoop();
}


void Core::display(void)
{ 

	c_pMainWindow->renderFrame();
	fpsCurrent++;

	// if(setUltrasound){
	// 	//glViewport(c_pMainWindow->m_nWidth,0,c_pMainWindow->m_nWidth,c_pMainWindow->m_nHeight);
	// 		 secondWindow->updateFromBratrack(c_pMainWindow->rotxUltrasound, c_pMainWindow->transducerOut);
	// 		// secondWindow->setScreen();
	// 		 //secondWindow->getSlice();
	// 		// secondWindow->display();
	// 	    //c_pMainWindow->setScreen();
	// }
}

void Core::displayUS(void){

	secondWindow->display();

}

void Core::idleUS(void){

	// glutSetWindow(secondWindow->secondScreen);
	// secondWindow->updateFromBratrack(c_pMainWindow->rotxUltrasound, c_pMainWindow->transducerOut);
	// secondWindow->getSlice();
	// glutPostRedisplay();
}

void Core::idle(void)
{ 
	//glutSetWindow(1);
	c_pMainWindow->idle();	
	glutSetWindow(2);
	secondWindow->updateFromBratrack(c_pMainWindow->rotxUltrasound, c_pMainWindow->transducerOut);
	secondWindow->getSlice();
	glutPostRedisplay();
}

void Core::reshape(int width, int height)
{ 
	c_pMainWindow->resize(width, height);
}

void Core::reshapeUS(int width, int height)
{ 
	secondWindow->resize(width, height);
}

void Core::keyboard(unsigned char key, int x, int y)  
{ 
	c_pMainWindow->keyEvent(key,x,y);
	//secondWindow->keyEvent(key);
}

void Core::mouse(int button, int state, int x, int y)
{ 
	c_pMainWindow->mouseButtonEvent(button, state,x,y);
}

void Core::move(int x, int y)                         
{ 
	c_pMainWindow->mouseMoveEvent(x,y);
}


void Core::countFPS(int value) {
	char title[120];
	fpsCount = fpsCurrent;
	fpsCurrent = 0;
	
	snprintf(title, 120, "%s. FPS: %d", "Image Match", fpsCount);
	glutSetWindowTitle(title);
	glutTimerFunc(1000, countFPS, 1);
}



