#include <GL/glut.h>  
#include <math.h>     
#include <stdio.h>
#include <time.h>
#define MAX_BALLS 7
#define RAND_NUM() ((double)rand()/RAND_MAX)

typedef struct BALL{
   float x, y, r;
   int state; 
   /* State je indikator da li loptica treba da se crta. */
}Ball;
 
const char title[] = "Rebbubled";  
int window_width  = 500;   
int window_height = 500;    
int window_pos_x   = 50;      
int window_pos_y   = 50;      

/* Fleg za koliziju. */
int hit_flag;

/* Flegovi za animaciju. */
static int animation_active;
static int arrow_animation;

/* Koordinate igraca, strele i "tajmera". */
static float player_x = 0.0;
static float player_y = -0.85;
static float arrow_x = 0.0;
static float arrow_y = -0.45;
static float timer_x = 0.98;

/* Brzina strele i "tajmera". */
static float arrow_speed = 0.05; 
static float timer_speed = 0.0001;

/* Brzine loptica. */
float vx[MAX_BALLS], vy[MAX_BALLS];

/* Koordinate loptica. */
float xpos[MAX_BALLS], ypos[MAX_BALLS];

/* Velicine precnika loptica. */
static float big_radius = 0.12;
static float medium_radius = 0.09;
static float small_radius = 0.06;

/* Niz loptica. */
Ball* balls[MAX_BALLS];

/* Granice. */  
float ball_x_max, ball_x_min, ball_y_max, ball_y_min;
float clip_area_x_left, clip_area_x_right, clip_area_y_bottom, clip_area_y_top;

int refresh = 30;

/* Deklaracije callback funkcija */
static void keyboard(unsigned char key, int x, int y);
static void reshape(int width, int height);
static void display(void);
static void timer(int value);

/* Funkcija koja proverava granice odskakanja lopte. */
static void bounce();
/* Funkcija koja proverava granice strele. */
static void shoot(void);
/* Funkcija koja proverava da li je igrac pogodjen loptom. */
static int hits(float *x, float *y, float *r);
/* Funkcija koja proverava da li je strela pogodila loptu.*/
static int success(float *xpos, float *ypos, float *r);
/* Funkcija koja pravi novu loptu. */
static Ball* new_ball(float *xbr, float *ybr, float rbr, int state);
/* Funkcija koja inicijalizuje loptice. */
static void init_balls();
/* Funkcija koja crta strelu. */
static void draw_arrow();
/* Funkcija koja crta igraca. */
static void draw_player();
/* Funkcija koja crta pozadinu. */
static void draw_background();
/* Funkcija koja crta loptice. */
static void draw_ball();
/* Funkcija koja crta pravougaonik koji se smanjuje 
   i predstavlja tajmer. */
static void draw_timer();

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

   glMaterialfv(GL_FRONT, GL_AMBIENT, no_material);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
   glMaterialfv(GL_FRONT, GL_EMISSION, no_material); 
   
   /* Crtamo lopte. */        
   draw_ball();
   
   /* Crtamo igraca. */
   draw_player();

   /* Iskljucujemo svetlo da bismo mogli da crtamo strele, 
      kao i pozadinu i tajmer. */

   glDisable(GL_LIGHTING);

   /* Crtamo strelu. */
   if(arrow_animation){
     draw_arrow();
   }

   /* Crtamo pozadinu. */
   draw_background();   
  
   /* Crtamo tajmer. */
   draw_timer(); 

   /*Ako je pogodjen igrac, izlazimo iz programa. */
   if(hit_flag == 1){
      exit(1);
   }
   
   /* Postavlja se nova slika u prozor. */
   glutSwapBuffers(); 
}

static void init_balls(){
   
   int i;
   for (i = 0; i < MAX_BALLS; i++) {

     /* Lopticama dodeljujemo random pocetne x i y koordinate,
        kao i brzine. */

     xpos[i] = -1.0 + RAND_NUM();
     ypos[i] = -1.0 + RAND_NUM();
     vx[i] = RAND_NUM() * 0.03 + 0.01;
     vy[i] = RAND_NUM() * 0.06 + 0.05;
      
     /* Prva lopta ima najveci radius. */
     if(i == 0){
        balls[i] = new_ball(&xpos[i], &ypos[i], big_radius, 1);
     }
     /* Sledece dve imaju manji. */
     if(i == 1 || i == 2){
        balls[i] = new_ball(&xpos[i], &ypos[i], medium_radius, 0);
     }
     /* Poslednje 4 imaju najmanji. */
     if(i > 2){
        balls[i] = new_ball(xpos, ypos, small_radius, 0);
     }
  }
}

static void draw_arrow(){
   
   glPushMatrix();
    glTranslatef(arrow_x, arrow_y, 0);
    glBegin(GL_LINES);
     glColor3f(0, 1, 1);
     glVertex3f(arrow_x, arrow_y - 0.1, 0);
     glVertex3f(arrow_x, arrow_y, 0);
    glEnd();
   glPopMatrix();

}

static void draw_player(){
    
   glPushMatrix();
    glTranslatef(player_x, player_y, 0);
    glColor3f(0, 0.5, 1);
    glutSolidCube(0.1);
   glPopMatrix();

}

static void draw_ball(){

   /* Crtamo samo one lopte kojima je indikator za crtanje 1.
      Inicijalno, samo je prvoj lopti indikator 1. */
   
   int i;
   glPushMatrix();
   for (i=0; i < MAX_BALLS; i++) {
       if(balls[i]->state == 1){
           glPushMatrix();
            glTranslatef(balls[i]->x, balls[i]->y, 0);
            glutSolidSphere(balls[i]->r, 15, 15);
           glPopMatrix();
        }  
     }

   glPopMatrix();
}

static void draw_background(){
 
   /* Crtamo sarenu pozadinu. */
   glPushMatrix();
    glBegin(GL_QUADS);	
     glColor3f(1.0, 0.0, 0.2);
     glVertex2f(-1.0, -0.9);  
     glVertex2f(1.0, -0.9);
     glColor3f(0.1, 0.4, 1.0);
     glVertex2f(1.0, 1.0);
     glVertex2f(-1.0, 1.0);
    glEnd();
   glPopMatrix();

}

static void draw_timer(){

   /* Crtamo gradijentni pravougaonik koji ce predstavljati tajmer.
      Njegova boja polako postaje crvena sto vreme vise odmice. */

   glPushMatrix();
    glBegin(GL_QUADS);
    glTranslatef(timer_x,0, 0);
     glColor3f(1.0, 0.7 + timer_x, 0.7 + timer_x);
     glVertex2f(-0.98, -0.92);
     glVertex2f(timer_x, -0.92);
     glColor3f(0, 0, 0.7 + timer_x);
     glVertex2f(timer_x, -0.98);
     glVertex2f(-0.98, -0.98);
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

static void bounce(){
   
   int i;
   for(i = 0; i < MAX_BALLS; i++){
       if(balls[i]->state == 1){
   
       /* Ubrzavamo loptice kojima je indikator
          za crtanje 1. */

       balls[i]->x += vx[i] * 0.04;
       balls[i]->y += vy[i] * 0.04;
   
       /* Granice za kretanje lopte. */
       ball_x_min = clip_area_x_left + balls[i]->r;
       ball_x_max = clip_area_x_right - balls[i]->r;
       ball_y_min = clip_area_y_bottom+0.1 + balls[i]->r;
       ball_y_max = clip_area_y_top/1.5 - balls[i]->r;

       /* Ponasanje lopti u zavisnosti od toga 
          da li su udarile u zid.*/

       if (balls[i]->x > ball_x_max) {
           balls[i]->x = ball_x_max;
           vx[i] = -vx[i];
       } else if (balls[i]->x < ball_x_min) {
           balls[i]->x = ball_x_min;
           vx[i]= -vx[i];
         }
       if (balls[i]->y > ball_y_max) {
           balls[i]->y = ball_y_max;
           vy[i] = -vy[i];
       } else if (balls[i]->y  < ball_y_min) {
           balls[i]->y  = ball_y_min;
           vy[i] = -vy[i];
         }
      }

   /* Proverava se da li je loptica udarila igraca. */
   
   if(hits(&balls[i]->x, &balls[i]->y, &balls[i]->r)){
      hit_flag = 1;
    //printf("Igrac pogodjen lopticom broj %d!\n", i);             
    } 
   }
   
   /* Smanjujemo x koordinatu pravougaonika koji predstavlja
      tajmer. Kada dodje do granice, vreme je isteklo i 
      izlazi se iz igre. */

   timer_x -= timer_speed; 

  /* + 0.02 zbog "ivice" pravougaonika. */

   if(timer_x < clip_area_x_left + 0.02){  
      printf("Igracu isteklo vreme za igru!\n");
      exit(1);
   }
   /* Forsira se ponovno iscrtavanje prozora. */
   glutPostRedisplay();
}

static void shoot(void){
	
   /* Ako strela predje y granicu,
      vracamo je dole i postaje neaktivna.	
      Vracamo je tacno "ispod" igraca. */

   if(arrow_y > clip_area_y_top){ 
      arrow_y = clip_area_y_bottom + 0.6;
      arrow_animation = 0;
   }
}

static Ball* new_ball(float *xbr, float *ybr, float rbr, int state){
   
   /* Kreiramo novu loptu. */
   Ball* new_ball = (Ball*)malloc(sizeof(Ball));
    
   if(new_ball == NULL){
	exit(EXIT_FAILURE);
   }
   new_ball->x = *xbr;
   new_ball->y = *ybr;
   new_ball->r = rbr;
   new_ball->state = state;
   return new_ball;
}

static void timer(int value)
{ 
   /* Povecava se y koordinata strele. */
   arrow_y += arrow_speed;

   /* Ispaljivanje strela. */
   shoot();
   
   /* Proverava se da li je postignut pogodak.
      U slucaju da je prva loptica pogodjena njen indikator
      za crtanje postavlja se na 0, dok se za sledece 
      loptice koje treba da se crtaju postavlja na 1. */

   int p;
   for(p = 0; p<MAX_BALLS; p++){
      if(success(&balls[p]->x, &balls[p]->y, &balls[p]->r) && balls[p]->state == 1) {
         if(p == 0){
            balls[p]->state = 0;        // Ako je pogodjena prva loptica,
            balls[p+1]->state = 1;      // treba da se pojave sledece dve manje.
            balls[p+2]->state = 1; 
            
         }
         if(p == 1){                 
            balls[p]->state = 0;        // Ako je pogodjena prva manja loptica,
            balls[p+2]->state = 1;      // treba da ostane druga manja loptica i da
            balls[p+3]->state = 1;      // se pojave dve manje od prve. 
	    
         }
         if(p == 2){                    // Ako je pogodjena druga manja loptica.
            balls[p]->state = 0;        // treba da ostane prva manja loptica i da
            balls[p+3]->state = 1;      // se pojave dve manje od druge.
            balls[p+4]->state = 1;
	    
         }
         if(p > 2){                     // Ako su pogodjene najmanje loptice, 
            balls[p]->state = 0;        // ne crtamo nove loptice.
            
         }
                
         //printf("Pogodjena loptica broj %d!\n", p);
    
     }
  }
 
   /* Forsira se ponovno iscrtavanje prozora. */
   glutPostRedisplay();        
 
  /* Postavlja se ponovo tajmer po potrebi. */
   if(arrow_animation){
      glutTimerFunc(refresh, timer, 0);
  }
	
}

static void keyboard(unsigned char key, int x, int y){
 
   switch(key){
   case 'd':
            /* Kretanje u desno sve dok ne dodjemo do granice. */
            if(player_x <= clip_area_x_right - 0.05){
            player_x += 0.02;
	    } 
	    break;
   case 'a':
            /* Kretanje u levo sve dok ne dodjemo do granice. */
	    if(player_x >= clip_area_x_left + 0.05){
	    player_x -= 0.02;
	    } 
	    break;
   case 'f': 
	    arrow_animation = 0;
 
            /* Nakon pucanja strele, postavljamo njenu x koordinatu
	       na x koordinatu igraca, delimo sa 2 kako bi bila 
               ispaljena sa "sredine" igraca. */
	   
            arrow_x = player_x/2;
            if(animation_active){
          	glutTimerFunc(refresh, timer, 0);
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

   /* Ne dozvoljavamo promene velicine prozora. */
   /* Nisam sigurna koliko je ovo korektno, ali neka ostane za sad. */
  
   glutReshapeWindow(500, 500);

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
   glutIdleFunc(bounce);
   glutDisplayFunc(display);    
   glutReshapeFunc(reshape);   
   glutKeyboardFunc(keyboard);
   glutTimerFunc(0, timer, 0);  
   animation_active = 1;
   
   /* Inicijalizacija loptica. */
   init_balls();
  
   /* Ulazi se u glavnu petlju */
   glutMainLoop();              
    
   return 0;
}
