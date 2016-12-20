#include <GL/glut.h>  
#include <math.h>     
#include <stdio.h>
#define MAX_BALLS 7

typedef struct BALL{
   float x, y, r;
   int state; 
   /* State je indikator da li loptica treba da se crta. */
}Ball;

static int animation_active;
static int arrow_animation;
static float arrow_speed = 0.05; 
static float arrow_x = 0.0;
static float arrow_y = -0.7;
const char title[] = "Rebbubled";  
int window_width  = 500;   
int window_height = 500;    
int window_pos_x   = 50;      
int window_pos_y   = 50;      
int p;
int hit_flag;
float player_x = 0.0;
float player_y = -0.95; 
float x_speed = 0.01;     
float y_speed = 0.04;
int refresh = 30;  
float ball_x_max, ball_x_min, ball_y_max, ball_y_min;
float clip_area_x_left, clip_area_x_right, clip_area_y_bottom, clip_area_y_top;
Ball* balls[MAX_BALLS];

/* Deklaracije callback funkcija */
static void keyboard(unsigned char key, int x, int y);
static void reshape(int width, int height);
static void display(void);
static void timer(int value);

/* Funkcija koja proverava granice odskakanja lopte. */
static void bounce(float *x, float *y, float *r);
/* Funkcija koja proverava granice strele. */
static void shoot(void);
/* Funkcija koja proverava da li je igrac pogodjen loptom. */
static int hits(float *x, float *y, float *r);
/* Funkcija koja proverava da li je strela pogodila loptu.*/
static int success(float *xpos, float *ypos, float *r);
/* Funkcija koja pravi novu loptu. */
static Ball* new_ball(float xbr, float ybr, float rbr, int state);
/* Funkcija koja crta strelu. */
static void draw_arrow();
/* Funkcija koja crta igraca. */
static void draw_player();
/* Funkcija koja crta pozadinu. */
static void draw_background();
/* Funkcija koja crta loptice. */
static void draw_ball(float* xpos, float* ypos, float* r);
/* Funkcija koja odredjuje ubrzanje loptica. */
static void speed(float *x, float *y);

void display(void) {

   /* Postavlja se boja svih piksela na zadatu boju pozadine. */
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
 
   glMatrixMode(GL_MODELVIEW);   
   glLoadIdentity();           

   /* Parametri svetla */
   GLfloat light_ambient[] = { 0, 0, 0, 1 };
   GLfloat light_diffuse[] = { 1, 1, 0.9, 1 };
   GLfloat light_specular[] = { 1, 1, 1, 1 };
   GLfloat model_ambient[] = { 0.4, 0.4, 0.4, 1 };
   
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

   /* Pozicija svetla. */
   GLfloat light_position[] = { 1, 1, 1, 0 };

   /* Nulti koeficijenti refleksije materijala. */
   GLfloat no_material[] = { 0, 0, 0, 1 };

   /* Koeficijenti ambijentalne refleksije materijala. */
   GLfloat material_ambient[] = { 0.7, 0.7, 0.7, 1 };

   /* Koeficijenti difuzne refleksije materijala. */
   GLfloat material_diffuse[] = { 0.1, 0.5, 0.8, 1 };

   /* Koeficijenti spekularne refleksije materijala. */
   GLfloat material_specular[] = { 1, 1, 1, 1 };

   /* Koeficijent spekularne refleksije za slucaj kada nema spekularne refleksije. */
   GLfloat no_shininess[] = { 0 };

   /* Koeficijent spekularne refleksije za slucaj male vrednosti koeficijenta. */
   GLfloat low_shininess[] = { 5 };

   /* Koeficijent spekularne refleksije za slucaj velike vrednosti koeficijenta. */
   GLfloat high_shininess[] = { 100 };

   /* Emisiona boja materijala. */
   GLfloat material_emission[] = { 0.3, 0.2, 0.2, 0 };

   /* Pozicionira se svetlo. */
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   /* Crtamo samo one lopte kojima je indikator za crtanje 1.
      Inicijalno, samo je prvoj lopti indikator 1. */

   for(p = 0; p < MAX_BALLS; p++){
       if(balls[p]->state == 1){
           glMaterialfv(GL_FRONT, GL_AMBIENT, no_material);
           glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
           glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
           glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
           glMaterialfv(GL_FRONT, GL_EMISSION, no_material); 
           draw_ball(&balls[p]->x, &balls[p]->y, &balls[p]->r);
       }
   }

   /*Ako je pogodjen igrac, izlazimo iz programa. */
   if(hit_flag == 1){
      exit(1);
   }
    
   /* Crtamo igraca. */
   draw_player();

   /* Iskljucujemo svetlo da bismo mogli da crtamo strele */
   glDisable(GL_LIGHTING);

   /* Crtamo strelu. */
   draw_arrow();
    
   /* Crtamo pozadinu. */
   draw_background();   
 
   /* Postavlja se nova slika u prozor. */
   glutSwapBuffers(); 
}


static void draw_arrow(){
   
   glPushMatrix();
    glTranslatef(player_x, arrow_y, 0);
    glBegin(GL_LINES);
     glColor3f(0, 1, 1);
     glVertex3f(arrow_x, arrow_y - 0.3, 0);
     glVertex3f(arrow_x, arrow_y, 0);
    glEnd();
   glPopMatrix();

}

static void draw_player(){
    
   glPushMatrix();
    glTranslatef(player_x, -0.95, 0);
    glColor3f(0, 0.5, 1);
    glutSolidCube(0.1);
   glPopMatrix();

}

static void draw_ball(float* xpos, float* ypos, float* r){
   
   glPushMatrix();
    glTranslatef(*xpos, *ypos, 0);
    glutSolidSphere(*r, 15, 15);
  glPopMatrix();

}

static void draw_background(){
 
   /* Crtamo sarenu pozadinu. */
   glPushMatrix();
    glBegin(GL_QUADS);	
     glColor3f(1.0, 0.0, 0.2);
     glVertex2f(-3.0, -3.0);  
     glVertex2f(3.0, -3.0);
     glColor3f(0.1, 0.4, 1.0);
     glVertex2f(3.0, 3.0);
     glVertex2f(-3.0, 3.0);
    glEnd();
   glPopMatrix(); 

}

static int hits(float *x, float *y, float *r){

   if((*x - player_x) * (*x - player_x) + 
      (*y - player_y) * (*y - player_y) <= 
      (*r) * (*r)){
	   return 1;
   }
   else return 0;

}

static int success(float *xpos, float *ypos, float *r){
   if((*xpos - player_x) * (*xpos - player_x) + 
      (*ypos - arrow_y) * (*ypos - arrow_y) <= 
      (*r) * (*r)){
	   return 1;   
   }
   else return 0;

}
static void speed(float *x, float *y){
 
   /* Kretanje loptice */
   *x += x_speed;
   *y += y_speed;

}

static void bounce(float *x, float *y, float *r){

   ball_x_min = clip_area_x_left + *r;
   ball_x_max = clip_area_x_right - *r;
   ball_y_min = clip_area_y_bottom + *r;
   ball_y_max = clip_area_y_top/1.2 - *r;

   /* Granice za kretanje lopte */
   if (*x > ball_x_max) {
       *x = ball_x_max;
       x_speed = -x_speed;
   } else if (*x < ball_x_min) {
       *x = ball_x_min;
       x_speed = -x_speed;
   }
   if (*y > ball_y_max) {
       *y = ball_y_max;
       y_speed = -y_speed;
   } else if (*y < ball_x_min) {
       *y = ball_x_min;
       y_speed = -y_speed;
   }

}

static void shoot(void){
	
   /* Ako strela predje y granicu, vracamo je dole i postaje neaktivna. */	

   if(arrow_y > clip_area_y_top){ 
      arrow_y = clip_area_y_bottom;
      arrow_animation = 0;
   }
}

static Ball* new_ball(float xbr, float ybr, float rbr, int state){
   
   Ball* new_ball = (Ball*)malloc(sizeof(Ball));
   
   if(new_ball == NULL){
	exit(EXIT_FAILURE);
   }
   new_ball->x = xbr;
   new_ball->y = ybr;
   new_ball->r = rbr;
   new_ball->state = state;
   return new_ball;
}

static void timer(int value)
{  
         
   
   switch (value) {
   case 1:
          /* Uvecavamo koordinate samo onih loptica kojima je indikator za crtanje 1. */
          for(p = 0; p < MAX_BALLS; p++){
              if(balls[p]->state == 1){
                 speed(&balls[p]->x, &balls[p]->y);
              }
          } 
 
          /* Definisemo kretanje samo onih loptica kojima je indikator za crtanje 1. */
          for(p = 0; p < MAX_BALLS; p++){
	      if(balls[p]->state == 1){
                 bounce(&balls[p]->x, &balls[p]->y, &balls[p]->r);
              }
          }
     
          /* Provera da li je igraca pogodila loptica. */
          for(p = 0; p < MAX_BALLS; p++){
              if(hits(&balls[p]->x, &balls[p]->y, &balls[p]->r) && balls[p]->state == 1){
                 hit_flag = 1;
             }
          }
           
          /* Forsira se ponovno iscrtavanje prozora. */
	  glutPostRedisplay();
	 
          /* Open se postavlja tajmer. */
	  glutTimerFunc(refresh, timer, 1);
	  break;

   case 2:
    
	  /* Povecava se y koordinata strele. */
	  arrow_y += arrow_speed;
		
          /* Ispaljivanje strela. */
	  shoot();

          /* Proverava se da li je postignut pogodak. */
	  if(success(&balls[0]->x, &balls[0]->y, &balls[0]->r)) {
	     balls[0]->state = 0;
             balls[1]->state = 1;
             balls[2]->state = 1;
             /* U slucaju da je prva loptica pogodjena njen indikator
		za crtanje postavlja se na 0, dok se za sledece dve 
		loptice postavlja na 1. */
          }
          
          /* TO DO: dodati proveru i za ostale loptice. */

          /* Forsira se ponovno iscrtavanje prozora. */
	  glutPostRedisplay(); 
 
       	  /* Postavlja se ponovo tajmer. */
	  if(arrow_animation){
	    glutTimerFunc(refresh, timer, 2);
	  }
	  break;
  }
}

static void keyboard(unsigned char key, int x, int y){
 
   switch(key){
   case 'd':
            if(player_x <= clip_area_x_right - 0.05){
            player_x += 0.02;
	    } 
	    break;
   case 'a':
	    if(player_x >= clip_area_x_left + 0.05){
	    player_x -= 0.02;
	    } 
	    break;
   case 'f': 
	    arrow_animation = 0; 
	    
            if(animation_active){
          	glutTimerFunc(refresh, timer, 2);
		arrow_animation = 1;
	    }          	
            break;
   case 27:
	   exit(1);
  }
}

void reshape(int width, int height) {

   if (height == 0) height = 1;    

   GLfloat aspect = (float)width / (float)height;
 
   glViewport(0, 0, width, height);
 
   glMatrixMode(GL_PROJECTION);  
   glLoadIdentity();           
   
   if (width >= height) {
       clip_area_x_left   = -1.0 * aspect;
       clip_area_x_right  = 1.0 * aspect;
       clip_area_y_bottom = -1.0;
       clip_area_y_top    = 1.0;
   } else {
       clip_area_x_left   = -1.0;
       clip_area_x_right  = 1.0;
       clip_area_y_bottom = -1.0 / aspect;
       clip_area_y_top    = 1.0 / aspect;
   }
   gluOrtho2D(clip_area_x_left, clip_area_x_right, clip_area_y_bottom, clip_area_y_top);

}

int main(int argc, char** argv) {

   /* Inicijalizija GLUT-a */
   glutInit(&argc, argv);          
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   
   /* Kreira se prozor. */ 
   glutInitWindowSize(window_width, window_height);  

   /* Prozor se pozicionira na sredinu ekrana. */
   glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - window_width)/2,
             		  (glutGet(GLUT_SCREEN_HEIGHT) - window_height)/2); 
 
   /* Naziv programa. */
   glutCreateWindow(title);       

   /* OpenGL inicijalizacija. */
   glClearColor (0.0, 0.0, 0.0, 0.0); 
   glEnable(GL_DEPTH_TEST);
 
   /* Registrovanje funkcija za obradu dogadjaja */
   glutDisplayFunc(display);    
   glutReshapeFunc(reshape);   
   glutKeyboardFunc(keyboard);
   glutTimerFunc(0, timer, 1);  
   animation_active = 1;
   
   /* Inicijalizacija loptica. */
   balls[0] = new_ball(0.0, 0.0, 0.1, 1);
   balls[1] = new_ball(0.3, 0.4, 0.06, 0);
   balls[2] = new_ball(0.1, 0.2, 0.06, 0);
   balls[3] = new_ball(0.2, 0.5, 0.03, 0);
   balls[4] = new_ball(0.4, 0.6, 0.03, 0);
   balls[5] = new_ball(0.1, 0.3, 0.03, 0);
   balls[6] = new_ball(0.1, 0.2, 0.03, 0);
    
   
   /* Ulazi se u glavnu petlju */
   glutMainLoop();              
    
   return 0;
}
