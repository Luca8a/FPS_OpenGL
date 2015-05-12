#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>

#ifdef _WIN32
#include "glut.h"
#elif __APPLE__
#include <GLUT/GLUT.h>
#endif
#include <cstring>
#include <string.h>
#include <stdlib.h>
#include <math.h> 
#include <stdio.h>
#include <cstdio>
#include <math.h>
#include <time.h>
#include "targa.h"
#include "SOIL.h"
#include <iostream>
#include <vector>
#include "glm.h"



#define BDEBUG true
#define DEBUG(s) if (BDEBUG) printf (s)
#define CAMINA 0.02
#define CORRE 0.05
#define NUM_BALAS 100
#define VEL_BALAS 0.08
#define VEL_ENEMIGO 0.01
#define CANT_ENEMIGOS 5
#define VIDA_INICIAL 20

// Tecla ESC Para salir 
#define ESC 27

//velocidad caminado
float speed = CAMINA;

//Tamaño pantalla 
float ratio;
float WindowWidth = 800;
float WindowHeight = 400;


// Posicion y movimiento de la camara
float x = 155.0, y = -1.0, z = 5.0;
float cameraMove = 0.0;
float cameraMove2 = 0.0;

float rotCamera = 0.0;
int oldTimeSinceStart = 0;
int deltaTime;

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
GLboolean isColision = false;

//Texturas
GLuint pisoTexId = 0;

//Luz ambiental
GLfloat ambientColor[] = { .3f, .3f, .3f, 1.0f };


//Balas
GLint siguienteBala = 0;
typedef struct _bala
{
	bool isActive = false;
	float pos[3];
	float dir[2];
	float dist = 0;
}Bala;
Bala balas[NUM_BALAS];
GLMmodel *balaModel;

//Enemigo
GLMmodel *enemigoModel;
typedef struct _enemigo
{
	bool isActive = true;
	int vida = 3;
	int dificultad;
	bool ataque=false;
	float pos[3];
	int danio=0;
	float dir[2];
	int timeAtaque = 0;
}Enemigo;
Enemigo femuto[CANT_ENEMIGOS];

//Puntaje
int puntaje = 0;
int vida=VIDA_INICIAL;



bool collisionaEnemigo(Enemigo e);
void iniciaEnemigo(Enemigo *e);
void Init()
{
	int x, y, d;
	srand(time(NULL));
	DEBUG("DEBUG:\t Entro a Init\n");
	imageColision = LoadTGA("laberinto2.tga", &x, &y, &d);
	imageColision[0];
	imageColision[250];
	laberinto = glmReadOBJ("laberinto3.obj");
	pisoTexId = SOIL_load_OGL_texture("piso.jpg", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO);
	glBindTexture(GL_TEXTURE_2D, pisoTexId);
	glEnable(GL_TEXTURE_2D);

	balaModel = glmReadOBJ("Pistol.obj");

	enemigoModel = glmReadOBJ("Femuto.obj");

	for (int i = 0; i < CANT_ENEMIGOS; i++)
	{
		iniciaEnemigo(&femuto[i]);
	}

	DEBUG("DEBUG:\t Salio a Init\n");
}

void iniciaEnemigo(Enemigo *e){
	e->dificultad = rand() % 3 + 1;
	int randommm = rand();

	e->pos[0] = rand() % 316;
	e->pos[1] = rand() % 376;
	e->danio = 0;
	e->ataque = false;
	e->isActive = true;
	while (collisionaEnemigo(*e))
	{
		e->pos[0] += 10;
		if (e->pos[0]>300){
			e->pos[0] = 10;
		}
	}


	
}
bool collisionaEnemigo(Enemigo e){
	xImagenColision = (e.pos[0] / 316.0F) * 1264.0F;
	yImagenColision = (e.pos[1] / 376.0F)*1504.0F;
	if (xImagenColision >= 0 && xImagenColision < 1264.0F && yImagenColision >= 0 && yImagenColision < 1504.0F)
	{
		GLint indiceColision = yImagenColision * 1264.0F * 3 + xImagenColision * 3;
		if (imageColision[indiceColision] != 0){
			return true;
		}


	}
	return false;

}
//Mover Balas y pintarlas
void actualizaBalas(){
	for (int i = 0; i < NUM_BALAS; i++)
	{
		if (balas[i].isActive){
			balas[i].pos[0] += (balas[i].dir[0])*VEL_BALAS*deltaTime;
			balas[i].pos[1] += (balas[i].dir[1])*VEL_BALAS*deltaTime;
			balas[i].dist += VEL_BALAS*deltaTime;
			if (balas[i].dist > 10){
				balas[i].isActive = false;
				balas[i].dist = 0;
			}
			glTranslatef(balas[i].pos[0], balas[i].pos[1], balas[i].pos[2]);

			glmDraw(balaModel, GLM_TEXTURE | GLM_SMOOTH |GLM_MATERIAL);
			//glutSolidTeapot(0.5);

			glTranslatef(-balas[i].pos[0], -balas[i].pos[1], -balas[i].pos[2]);
			
		}
	}
}

int checaHit(Enemigo e){
	int vida = e.danio;
	for (int i = 0; i < NUM_BALAS; i++)
	{	
		if (balas[i].isActive){
			GLfloat d1, d2, dist;
			d1 = e.pos[0] - balas[i].pos[0];
			d2 = e.pos[1] - balas[i].pos[1];
			dist = sqrtf((d1*d1) + (d2*d2));
			if (dist <= 15.0f){
				vida += 1;
				balas[i].isActive = false;
			}
		}
	}
	return vida;
}
GLfloat calculaDistancia(Enemigo e){
	GLfloat d1, d2, dist;
	d1 = e.pos[0] - x;
	d2 = e.pos[1] - y;
	dist = sqrtf((d1*d1) + (d2*d2));
	return dist;
}
void mueveEnemigo(Enemigo *e){
	GLfloat dist = calculaDistancia(*e);
	if (dist < 400.0){
		if (dist < 10){
			e->ataque = true;
			e->timeAtaque = glutGet(GLUT_ELAPSED_TIME);
			vida--;
		}
		else
		{
			if (collisionaEnemigo(*e)){
				e->pos[0] -= ((x - e->pos[0]) / dist)*VEL_ENEMIGO*deltaTime;
				e->pos[1] -= ((y - e->pos[1]) / dist)*VEL_ENEMIGO*deltaTime;
			}
			else
			{
				e->pos[0] += ((x - e->pos[0]) / dist)*VEL_ENEMIGO * deltaTime;
				e->pos[1] += ((y - e->pos[1]) / dist)*VEL_ENEMIGO*deltaTime;
				if (collisionaEnemigo(*e)){

					e->pos[1] -= ((y - e->pos[1]) / dist)*VEL_ENEMIGO*deltaTime;
					if (collisionaEnemigo(*e)){
						e->pos[0] -= ((x - e->pos[0]) / dist)*VEL_ENEMIGO * deltaTime;
						e->pos[1] += ((y - e->pos[1]) / dist)*VEL_ENEMIGO*deltaTime * 2;

						if (collisionaEnemigo(*e)){
							e->pos[0] -= ((x - e->pos[0]) / dist)*VEL_ENEMIGO * deltaTime;
							e->pos[1] -= ((y - e->pos[1]) / dist)*VEL_ENEMIGO*deltaTime * 3;

						}
					}
				}
			}
		}
	}
}

void dibujaEnemigo(Enemigo e){
	glTranslatef(e.pos[0], 0, -e.pos[1]);
	glScalef(0.05, 0.05, 0.05);

	GLfloat alpha = (e.dificultad*e.vida - e.danio )/ (e.dificultad*e.vida);
	float m = (e.dificultad*e.vida);
	float vi = m - e.danio;
	float a = vi /m;

	glColor3f(a,a,a);

	glmDraw(enemigoModel, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);

	glScalef(1 / 0.05, 1 / 0.05, 1 / 0.05);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Sombra



	glColor4f(0.2, 0.2, 0.2, 0.5);
	glScalef(1, 0.005, 1);
	glutSolidSphere(5, 10, 10);
	glScalef(1, 1 / 0.005, 1);

	glDisable(GL_BLEND);

	glColor3f(1, 1, 1);
	glTranslatef(-e.pos[0], 0, +e.pos[1]);
}
void actualizaEnemigo(){


	for (int i = 0; i < CANT_ENEMIGOS; i++)
	{
		femuto[i].danio = checaHit(femuto[i]);
		if (femuto[i].isActive){



			if (femuto[i].danio < femuto[i].vida*femuto[i].dificultad){
				if (femuto[i].ataque){
					int dTime = glutGet(GLUT_ELAPSED_TIME) - femuto[i].timeAtaque ;
					if (dTime > 5000){
						femuto[i].ataque = false;
					}
				}
				else
				{
					mueveEnemigo(&femuto[i]);
				}
				dibujaEnemigo(femuto[i]);
				
			}
			else
			{
			


				puntaje += femuto[i].dificultad;
				iniciaEnemigo(&femuto[i]);
			}
		}
	}
}



//Colision de camara con laberinto
void colision(float auxX, float auxY){
	//leer la imagen y hacer la comparación de color 0 para negro y 1 para blanco
	xImagenColision = (auxX / 316.0F) * 1264.0F;
	yImagenColision = (auxY / 376.0F)*1504.0F;

	isColision = false;
	if (xImagenColision >= 0 && xImagenColision<1264.0F && yImagenColision >= 0 && yImagenColision<1504.0F)
	{

		
		indiceColision = yImagenColision * 1264.0F * 3 + xImagenColision * 3;
		//color =imageColision[i];
		
		if (imageColision[indiceColision] != 0){
			isColision = true;
			//printf("colisiona \n");
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
	WindowHeight = h;
	WindowWidth = w;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, ratio, 0.1, 300.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
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


		lz += speed*deltaTime;
		auxX += cameraMove * lx * speed*deltaTime;
		auxY += cameraMove * ly * speed*deltaTime;
			auxZ = cos(lz/5)*0.05 + 5;    

	}
	if (cameraMove2){

		auxX += cameraMove2 * dx * speed*deltaTime;
		auxY += cameraMove2 * dy * speed*deltaTime;
	}
	colision(auxX, auxY);
	if (!isColision){
		x = auxX;
		y = auxY;
		z = auxZ;
	}
	glutPostRedisplay(); 
}

//IMPRIMIR TEXTO
void printtext(int x, int y, char * string)
{
	//(x,y) is from the bottom left of the window
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(20, WindowWidth, 0, WindowHeight, -0.5f, 2.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glRasterPos2i(x, y);
	for (int i = 0; i<strlen(string); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
	}
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void renderScene()
{
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = timeSinceStart-oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;


	glClearColor(0.555, 0.555, 0.99900, 1.0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);


	glLoadIdentity();


	//Camara
	gluLookAt(
		x, y, 5.0,
		x + lx, y + ly, z,
		0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	

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

	
	

	actualizaBalas();

	glColor3f(1.0, 1.0, 1.0);
	glRotatef(90, 1, 0, 0);

	actualizaEnemigo();

	glTranslatef(1,0,0);



	//laberinto
	

	glmDraw(laberinto, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
	
	glColor3f(0, 0, 0);
	char result[16];
	sprintf(result, "Score:%d", puntaje);
	printtext(WindowWidth - 100, WindowHeight - 50, result);

	glColor3f(0, 0, 0);
	char vidas[16];
	sprintf(vidas, "VIDA:%d", vida);
	printtext(100,  50, vidas);
	

	glutSwapBuffers(); 
}

void dispara();
void presionarTeclasTeclado(unsigned char key, int xx, int yy)
{
	if (key == ESC) exit(0);
	switch (key) {
	case 'w': cameraMove = 1.0; break;
	case 's': cameraMove = -1.0; break;

		// Movimiento Lateral
	case 'a': cameraMove2 = -1.0; break;
	case 'd': cameraMove2 = 1.0; break;

	case ' ': speed = CORRE;; break;
	}

}

/*
* Disparar balas 
* Metodo que se llama cuando se presiona espacio 
*/
void dispara(){
	//Activar Bala
	balas[siguienteBala].isActive = true;

	//Pos Inicial de la bala
	balas[siguienteBala].pos[0] = x;
	balas[siguienteBala].pos[1] = y;
	balas[siguienteBala].pos[2] = z;

	//Dir de la bala

	balas[siguienteBala].dir[0] = lx;
	balas[siguienteBala].dir[1] = ly;

	siguienteBala++;
	if (siguienteBala == NUM_BALAS)siguienteBala = 0;


}


void teclaNoPresionada(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w': cameraMove = 0.0; break;
	case 's': cameraMove = 0.0; break;

		//Movimiento lateral
	case 'a': cameraMove2 = 0.0; break;
	case 'd': cameraMove2 = 0.0; break;
	case ' ': speed = CAMINA; break;

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
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			isDragging = 1; 
			xDragStart = x;
		}
		else { 
			angle += deltaAngle;
			isDragging = 0; 
		}
	}
	if (button == GLUT_LEFT_BUTTON){
		if (state == GLUT_DOWN){
			 dispara();
		}
	}

}


int main(int argc, char **argv)
{

	printf("Luis Carlos Ochoa Argüelles\nMiguel Hernandez\nDaniel Ruiz Bustos\n");
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WindowWidth, WindowHeight);
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