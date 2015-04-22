

#include <stdlib.h>
#include <math.h> 
#include <stdio.h> 
#include "glut.h"

// Tecla ESC Para salir 
#define ESC 27

float speed = 0.2;

// Posicion y movimiento de la camara
float x = 0.0, y = -5.0, z=1;
float cameraMove = 0.0;
float cameraMove2 = 0.0;

// Angulo de la camara
float lx = 0.0, ly = 1.0, lz= 0.0; // A donde apunta la camara
float dx = 1.0, dy = 0.0;
float angle = 0.0; // Angulo de rotacion de la camara
float deltaAngle = 0.0; // Angulo al arrastrar el mouse 

// Contorl del mouse
int isDragging = 0; // verdadero cuando se esta arrastrando
int xDragStart = 0; // cambio en x cuando arrastrando el mouse

// Cambio de tamaño 
void changeSize(int w, int h)
{
	float ratio = ((float)w) / ((float)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, ratio, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void objetoX()
{
	// Draw body (a 20x20 spherical mesh of radius 0.75 at height 0.75)
	glColor3f(0.0, 0.0, 0.0); // set drawing color to white
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.75);
	glutSolidTeapot(.75);
	glPopMatrix();

}

void update(void)
{
	if (cameraMove) { // update camera position
		x += cameraMove * lx * speed;
		y += cameraMove * ly * speed;
		// z = cos()*0.2+1; // TODO: cos de Que   
	}
	if (cameraMove2){
		x += cameraMove2 * dx * speed;
		y += cameraMove2 * dy * speed;
	}
	glutPostRedisplay(); // redisplay everything
}

void renderScene(void)
{
	int i, j;

	// Clear color and depth buffers
	glClearColor(1.0, 1.0, 1.0, 1.0); // sky color is light blue
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Set the camera centered at (x,y,1) and looking along directional
	// vector (lx, ly, 0), with the z-axis pointing up
	gluLookAt(
		x, y, 1.0,
		x + lx, y + ly, z,
		0.0, 0.0, 1.0);

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex3f(-100.0, -100.0, 0.0);
	glVertex3f(-100.0, 100.0, 0.0);
	glVertex3f(100.0, 100.0, 0.0);
	glVertex3f(100.0, -100.0, 0.0);
	glEnd();

	// Dibujar Objetos para referencia
	for (i = -3; i < 3; i++)
		for (j = -3; j < 3; j++) {
			glPushMatrix();
			glTranslatef(i*7.5, j*7.5, 0);
			objetoX();
			glPopMatrix();
		}

	glutSwapBuffers(); // Make it all visible
}


void presionarTeclasTeclado(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q') exit(0);
	switch (key) {
	case 'w': cameraMove = 1.0; break;
	case 's': cameraMove = -1.0; break;
	case 'a': cameraMove2 = -1.0; break;
	case 'd': cameraMove2 = 1.0; break;
	case ' ': speed = 0.7;  break;
	}
}




void teclaNoPresionada(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w': cameraMove = 0.0; break;
	case 's': cameraMove = 0.0; break;
	case 'a': cameraMove2 = 0.0; break;
	case 'd': cameraMove2 = 0.0; break;
	case ' ': speed = 0.2;  break;
	}
}

void mouseMove(int x, int y)
{
	if (isDragging) { // only when dragging
		// update the change in angle
		deltaAngle = (x - xDragStart) * 0.005;

		// camera's direction is set to angle + deltaAngle
		lx = -sin(angle + deltaAngle);
		ly = cos(angle + deltaAngle);

		lz = -sin(angle + deltaAngle);

		dx = cos(angle + deltaAngle);
		dy = sin(angle + deltaAngle);
	}
}

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) { // left mouse button pressed
			isDragging = 1; // start dragging
			xDragStart = x; // save x where button first pressed
		}
		else { /* (state = GLUT_UP) */
			angle += deltaAngle; // update camera turning angle
			isDragging = 0; // no longer dragging
		}
	}
}

int main(int argc, char **argv)
{


	// general initializations
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 400);
	glutCreateWindow("OpenGL/GLUT Sampe Program");

	// register callbacks
	glutReshapeFunc(changeSize); // window reshape callback
	glutDisplayFunc(renderScene); // (re)display callback
	glutIdleFunc(update); // incremental update
	glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
	glutMouseFunc(mouseButton); // process mouse button push/release
	glutMotionFunc(mouseMove); // process mouse dragging motion
	glutKeyboardFunc(presionarTeclasTeclado); // process standard key clicks
	glutKeyboardUpFunc(teclaNoPresionada);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 0; // this is just to keep the compiler happy
}