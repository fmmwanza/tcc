#include "transdutor.h"

void drawTransducer(){
	glBegin(GL_TRIANGLES);
		// drawing front face, starting from top going clockwise
		glColor3f(1.0f, 0.0f, 0.0f); // red
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		// drawing right face, starting from top going clockwise
		glColor3f(0.0f, 1.0f, 0.0f); // green
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);

		// drawing back face, starting from top going clockwise
		glColor3f(0.0f, 0.0f, 1.0f); // blue
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		
		// drawing left face, starting from top going clockwise
		glColor3f(1.0f, 0.0f, 1.0f); // purple
		glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
	glEnd();
}

