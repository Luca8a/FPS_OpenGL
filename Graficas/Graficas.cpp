

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
float lx = 0.0, ly = 1.0; // A donde apunta la camara
float lz = 0.0;
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


//TODO : Modelado de los enemigos
void objetoX()
{
	glColor3f(0.0, 0.0, 0.0); 
	glPushMatrix();
	glTranslatef(0.0, 0.0, 2.0);
	glutSolidTeapot(.75);
	glPopMatrix();

}

void update()
{
	if (cameraMove) { // update camera position
		lz+=speed;
		x += cameraMove * lx * speed;
		y += cameraMove * ly * speed;
		z = cos(lz)*0.05+1; // TODO: cos de Que   
	}
	if (cameraMove2){
		x += cameraMove2 * dx * speed;
		y += cameraMove2 * dy * speed;
	}
	glutPostRedisplay(); 
}

void renderScene()
{
	int i, j;


	glClearColor(1.0, 1.0, 1.0, 1.0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();


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


	for (i = -3; i < 3; i++)
		for (j = -3; j < 3; j++) {
			glPushMatrix();
			glTranslatef(i*7.5, j*7.5, 0);
			objetoX();
			glPopMatrix();
		}

	glutSwapBuffers(); 
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
	if (isDragging) { 

		deltaAngle = (x - xDragStart) * 0.005;


		lx = -sin(angle + deltaAngle);
		ly = cos(angle + deltaAngle);

		dx = cos(angle + deltaAngle);
		dy = sin(angle + deltaAngle);
	}
}

void mouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			isDragging = 1; 
			xDragStart = x;
		}
		else { 
			angle += deltaAngle;
			isDragging = 0; 
		}
	}
}

int main(int argc, char **argv)
{


	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 400);
	glutCreateWindow("FPS Graficas Computacionales");

	
	glutReshapeFunc(changeSize); 
	glutDisplayFunc(renderScene); 
	glutIdleFunc(update); 
	glutIgnoreKeyRepeat(1);
	glutMouseFunc(mouseButton); 
	glutMotionFunc(mouseMove); 
	glutKeyboardFunc(presionarTeclasTeclado); 
	glutKeyboardUpFunc(teclaNoPresionada);


	glEnable(GL_DEPTH_TEST);


	glutMainLoop();

	return 0; 
}