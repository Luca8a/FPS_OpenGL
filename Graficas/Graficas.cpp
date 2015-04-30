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
#define CAMINA 0.05
#define CORRE 0.2

// Tecla ESC Para salir 
#define ESC 27

//velocidad caminado
float speed = CAMINA;

//Tamaño pantalla 
float ratio;


// Posicion y movimiento de la camara
float x = 155.0, y = -1.0, z=5.0;
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
GLboolean isColision=false;

//Texturas
GLuint pisoTexId = 0;

//Luz ambiental
GLfloat ambientColor[] = { .3f, .3f, .3f, 1.0f };

void Init()
{
	int x, y, d;
	DEBUG("DEBUG:\t Entro a Init\n");
	imageColision = LoadTGA("laberinto2.tga", &x, &y, &d);
	imageColision[0];
	imageColision[250];
	laberinto = glmReadOBJ("laberinto3.obj");
	pisoTexId = SOIL_load_OGL_texture("piso.jpg",SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO);
	glBindTexture(GL_TEXTURE_2D, pisoTexId);
	glEnable(GL_TEXTURE_2D);
	DEBUG("DEBUG:\t Salio a Init\n");
}


void colision(float auxX, float auxY){
	//leer la imagen y hacer la comparación de color 0 para negro y 1 para blanco
	xImagenColision = (auxX / 316.0F) * 1264.0F;
	yImagenColision = (auxY / 376.0F)*1504.0F;

	isColision = false;
	if (xImagenColision >= 0 && xImagenColision<1264.0F && yImagenColision >= 0 && yImagenColision<1504.0F)
	{

		
		indiceColision = yImagenColision * 1264.0F * 3 + xImagenColision * 3;
		//color =imageColision[i];
		printf("pixel %d %d %d \n", imageColision[indiceColision], imageColision[indiceColision + 1], imageColision[indiceColision + 2]);
		if (imageColision[indiceColision] != 0){
			isColision = true;
			printf("colisiona \n");
		}

	}

	/*if(imageColision[i]!=0){
	col=true;
	}*/
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

	float auxX=x, auxY=y,auxZ=z;
	if (rotCamera){
		deltaAngle += rotCamera;
		lx = -sin(angle + deltaAngle);
		ly = cos(angle + deltaAngle);
	}
	if (cameraMove) { // update camera position


			lz += speed;
			auxX += cameraMove * lx * speed;
			auxY += cameraMove * ly * speed;
			auxZ = cos(lz/5)*0.05 + 5; // TODO: cos de Que   

	}
	if (cameraMove2){

		auxX += cameraMove2 * dx * speed;
		auxY += cameraMove2 * dy * speed;
	}
	colision(auxX, auxY);
	if (!isColision){
		x = auxX;
		y = auxY;
		z = auxZ;
	}
	glutPostRedisplay(); 
}

void renderScene()
{
	int i, j;


	glClearColor(0.555, 0.555, 0.99900, 1.0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);


	glLoadIdentity();

	//Camara
	gluLookAt(
		x, y, 5.0,
		x + lx, y + ly, z,
		0.0, 0.0, 1.0);
	
	//piso
	glColor3f(1.0, 1.0, 1.0);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pisoTexId);
	glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);

		glTexCoord2d(0.0, 1);
		glVertex3f(0.0, 376.0, 0.0);

		glTexCoord2d(1, 1);
		glVertex3f(316.0, 376.0, 0.0);

		glTexCoord2d(1.0, 0.0);
		glVertex3f(316.0, 0.0, 0.0);
	glEnd();


	glEnable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
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
	case ' ': speed = CORRE;  break;
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
	case ' ': speed = CAMINA;  break;
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