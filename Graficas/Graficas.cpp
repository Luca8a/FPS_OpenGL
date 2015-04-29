#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>

#ifdef _WIN32
#include "glut.h"
#elif __APPLE__
#include <GLUT/GLUT.h>
#endif

#include <math.h> 
#include <stdio.h>
#include <math.h>
#include "targa.h"
#include "SOIL.h"
#include <iostream>
#include <vector>
#include "glm.h"
#define BDEBUG true
#define DEBUG(s) if (BDEBUG) printf (s)


// Tecla ESC Para salir 
#define ESC 27

//velocidad caminado
float speed = 0.2;

//Tamaño pantalla 
float ratio;


// Posicion y movimiento de la camara
float x = 0.0, y = -5.0, z=1;
float cameraMove = 0.0;
float cameraMove2 = 0.0;

float rotCamera = 0.0;

// Angulo de la camara
float lx = 0.0, ly = 1.0; // A donde apunta la camara
float lz = 0.0;
float dx = 1.0, dy = 0.0;
float angle = 0.0; // Angulo de rotacion de la camara
float deltaAngle = 0.0; // Angulo al arrastrar el mouse 

// Contorl del mouse
int isDragging = 0; // verdadero cuando se esta arrastrando
int xDragStart = 0; // cambio en x cuando arrastrando el mouse


//Laberinto
GLMmodel *laberinto;

//Variables para la colision
GLint indiceColision, xImagenColision, yImagenColision, colorImagenColision;
GLubyte *imageColision;


//Luz ambiental
GLfloat ambientColor[] = { .3f, .3f, .3f, 1.0f };

void Init()
{
	int x, y, d;
	DEBUG("DEBUG:\t Entro a Init\n");
	imageColision = LoadTGA("laberinto.tga", &x, &y, &d);
	laberinto = glmReadOBJ("laberinto.obj");
	DEBUG("DEBUG:\t Salio a Init\n");
}


// Cambio de tamaño 
void changeSize(int w, int h)
{
	ratio= ((float)w) / ((float)h);
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
	glTranslatef(0.0, 0.0, 2.0);
	glutSolidTeapot(.75);
	glPopMatrix();

}

void update()
{
	if (rotCamera){
		deltaAngle += rotCamera;
		lx = -sin(angle + deltaAngle);
		ly = cos(angle + deltaAngle);
	}
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
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);


	glLoadIdentity();

	//Camara
	gluLookAt(
		x, y, 1.0,
		x + lx, y + ly, z,
		0.0, 0.0, 1.0);
	
	////piso
	//glColor3f(1.0, 0.0, 0.0);
	//glBegin(GL_QUADS);
	//glVertex3f(-100.0, -100.0, 0.0);
	//glVertex3f(-100.0, 100.0, 0.0);
	//glVertex3f(100.0, 100.0, 0.0);
	//glVertex3f(100.0, -100.0, 0.0);
	//glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glRotatef(90, 1, 0, 0);
	glTranslatef(1,0,0);
	//laberinto
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	glmDraw(laberinto, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
	

	glutSwapBuffers(); 
}


void presionarTeclasTeclado(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q') exit(0);
	switch (key) {
	case 'w': cameraMove = 1.0; break;
	case 's': cameraMove = -1.0; break;

		// Movimiento Lateral
	case 'a': cameraMove2 = -1.0; break;
	case 'd': cameraMove2 = 1.0; break;

		
		break;
	case ' ': speed = 0.7;  break;
	}
}




void teclaNoPresionada(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w': cameraMove = 0.0; break;
	case 's': cameraMove = 0.0; break;

		//Movimiento lateral
	case 'a': cameraMove2 = 0.0; break;
	case 'd': cameraMove2 = 0.0; break;
		

		break;
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

	printf("Luis Carlos Ochoa Argüelles\nMiguel Hernandez\nDaniel Ruiz Bustos\n");
	
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

	Init();
	glutMainLoop();
	scanf("%d",&x);
	return 0; 
}