#include <GL/glut.h>
#include <math.h>
#define RIGHT 1
#define LEFT -1
#define UP 1
#define DOWN -1

/* Cuvamo velicinu ekrana */
static float width = 500.0;
static float height = 500.0;

/* Definisemo smerove */
static float directionX = RIGHT;
static float directionY = UP;

static float positionX = 1.0;
static float positionY = 0.5;

static float radius = 40;
static float playerx = 0.0;
/*Deklaracije callback funkcija*/
static void keyboard(unsigned char key, int x, int y);
static void reshape(int width, int height);
static void display(void);

/*Metod koji crta lopticu*/
static void display_ball(void);
/*Metod koji crta igraca*/
static void display_player(void);
/*Metod koji definise skakanje loptice*/
static void bounce(void);
/*Metod koji proverava da li je loptica presla opsege*/
static void ball_crossed_limits();

void display_ball(void){

	/* Crtamo lopticu */
        glPushMatrix(); 
	glColor3f(1.0, -0.5, 0.3);
	glTranslatef(positionX, positionY, 0.0);
	glBegin(GL_POLYGON);
	float angle;
	for(angle = 0; angle < 360; angle++){
		float r_angle = (2*angle*3.14)/360;
		glVertex2f(cos(r_angle)*radius, sin(r_angle)*radius);
	}

	glEnd();
	glPopMatrix();

}

void display_player(void){
	
	glPushMatrix();
	glLoadIdentity();

	/*Crtamo igraca koji je u ovom trenutku kvadrat i pomeramo ga na dno ekrana*/
	glTranslatef(playerx, -480, 0);
  	glColor3f(0, 0, 1);
	  glBegin(GL_QUADS); 
           glVertex3f(-20, -20, 0);
           glVertex3f(-20, 20, 0);
           glVertex3f(20, 20, 0);
           glVertex3f(20, -20, 0);
          glEnd();
	glPopMatrix();

}

void display(void)
{
	/* Postavlja se boja svih piksela na zadatu boju pozadine. */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Poziva se funkcija za crtanje loptice */
	display_ball();

	/* Poziva se funkcija za crtanje igraca */
	display_player();

	 /* Postavlja se nova slika u prozor. */
	glutSwapBuffers();
}

void ball_crossed_limits(){

	/* Proveravamo da li je loptica presla opsege i menjamo joj smer */
	if (width - radius == positionX){
		directionX = LEFT;
	}
	if (height - radius < positionY){
		directionY = DOWN;
	}
	if (-width + radius == positionX){
		directionX = RIGHT;
	}
	if (-height + radius > positionY){
		directionY = UP;
	}
}

void bounce(void)
{
	ball_crossed_limits();

	/*Definisemo kako ce loptica odskakati*/
	if (directionX == LEFT){
	   if (directionY == UP){
		positionX--;
		positionY+=5;
	   }
	   else{
		positionX--;
		positionY-=5;
	   }
	}
        else{
	   if (directionY == UP){
		positionX++;
		positionY+=5;
	   }
	   else{
	        positionX++;
		positionY-=5;
	    }
	}

	/* Ponovno iscrtavanje prozora */
	glutPostRedisplay();

}

void reshape(int w, int h)
{
   /* Postavlja se viewport */
   glViewport (0, 0, w, h);
	
   /* Postavljanje parametara projekcije */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   
   /* Ortogonalna projekcija */
   glOrtho(-width, width, -height, height, -1, 1);
   glMatrixMode(GL_MODELVIEW);
}

static void keyboard(unsigned char key, int x, int y)
{
   /* Kretanje igraca u desno */
   if(key == 'd'){
	if(playerx < width-20){
	playerx+=5;
	}
   } /*Kretanje igraca u levo */
   else if(key == 'a'){
	if(playerx > -width+20){
	playerx-=5;
	}
   } /*Izlazak iz programa */
   if(key == 27){
   exit(1);
 }
}

int main(int argc, char** argv)
{  
   /* Inicijalizija GLUT-a */
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DOUBLE);

   /* Kreira se prozor */
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);

   /* OpenGL inicijalizacija. */
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT); 

   /* Registrovanje funkcija za obradu dogadjaja */
   glutDisplayFunc(display);  
   glutKeyboardFunc(keyboard);
   glutReshapeFunc(reshape);
   glutIdleFunc(bounce);

   /* Ulazi se u glavnu petlju */
   glutMainLoop();
 
   return 0;
}
