#include <GL/glut.h> 
#include <math.h>     
#include <stdio.h>
#include <time.h>
#include <string.h>
#define MAX_BALLS 14

typedef struct BALL{
   float x, y, r;
   int state; /* State je indikator da li loptica treba da se crta. */ 
   char size; /* Large, medium ili small velicina. */
   float red;  
   float green;
   float blue;
}Ball;
 
const char title[] = "Rebbubled";  
int window_width  = 500;   
int window_height = 500;    

/* Fleg za koliziju. */
int hit_flag;

/* Flegovi za animaciju. */
static int animation_active;
static int animation_parameter;
static int arrow_animation;
static int start_flag;
static int pause_flag;

/* Koordinate igraca, strele i "tajmera". */
static float player_x = 0.0;
static float player_y = -0.85;
static float arrow_x = 0.0;
static float arrow_y = -0.45;
static float timer_x = 0.98;

/* Brzina strele i "tajmera". */
static float arrow_speed = 0.05; 
static float timer_speed = 0.0005;

/* Brzine loptica. */
float vx[MAX_BALLS], vy[MAX_BALLS];

/* Koordinate loptica. */
float xpos[MAX_BALLS], ypos[MAX_BALLS];

/* Boje loptica. */
float red[MAX_BALLS], green[MAX_BALLS], blue[MAX_BALLS];

/* Velicine precnika loptica. */
static float big_radius = 0.12;
static float medium_radius = 0.09;
static float small_radius = 0.06;

/* Niz loptica. */
Ball* balls[MAX_BALLS];

/* Granice. */  
float ball_x_max, ball_x_min, ball_y_max, ball_y_min;
float clip_area_x_left = -1;
float clip_area_x_right = 1;
float clip_area_y_bottom = -1;
float clip_area_y_top = 1;

int refresh = 30;
int score_value;
int num_balls_hit;

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
static Ball* new_ball(float *xbr, float *ybr, float rbr, int state, char size, float r, float g, float b);
/* Funkcija koja inicijalizuje pocetne koordinate loptica i njihove brzine. */
static void init_values();
/* Funkcija koja inicijalizuje loptice. */
static void init_balls();
/* Funkcija koja crta strelu. */
static void draw_arrow();
/* Funkcije koje crtaju igraca. */
static void draw_player_head();
static void draw_player_body();
static void draw_player_horns();
/* Funkcija koja crta pozadinu. */
static void draw_background();
/* Funkcija koja crta loptice. */
static void draw_ball();
/* Funkcija koja crta pravougaonik koji se smanjuje 
   i predstavlja tajmer. */
static void draw_timer();
/* Funkcija koja ispisuje poruke na ekranu. */
static void print_text(char *text, float x, float y);
/*Funkcija koja ispisuje trenutni rezultat na ekranu. */
static void print_score_value(float x, float y);
/** Postavljaju se parametri svetla.*/
static void init_light();
/* Funkcija koja vraca random broj u nekom intervalu. */
static double randf(double min, double max);

void display(void) {

   /* Postavlja se boja svih piksela na zadatu boju pozadine. */
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
 
   glMatrixMode(GL_MODELVIEW);   
   glLoadIdentity();          
   
   init_light();
 
   /* Pozicija svetla. */
   GLfloat light_position[] = { 1, 1, 1, 0 };

   /* Nulti koeficijenti refleksije materijala. */
   GLfloat no_material[] = { 0, 0, 0, 1 };

   /* Koeficijenti spekularne refleksije materijala. */
   GLfloat material_specular[] = { 0.8, 0.8, 0.8, 1 };

   /* Koeficijent spekularne refleksije za slucaj male vrednosti koeficijenta. */
   GLfloat low_shininess[] = { 15 };

   /* Pozicionira se svetlo. */
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   
   /* U slucaju da je start flag postavljen na 0, igrica jos uvek nije pokrenuta,
      ispisujemo poruku "Press s to start." */
   if(!start_flag){
      print_text("Press s to start", -0.3, 0.8);
   }
   /* Ako je igrica pauzirana, ispisujemo odgovarajuce poruke. */
   else if(pause_flag){
      print_text("Game paused", -0.2, 0.8);
      print_text("Press r to resume", -0.3, 0.7);
   }
   /* Inace, ispisujemo ostalo. */
   else{
      /* Ispisujemo tekst u gornjem levom cosku. */
      print_text("Score: ", -0.95, 0.9);

      /* Ispisujemo trenutni rezultat pored teksta. */
      print_score_value(-0.7, 0.9);

      /* Kada pogodimo sve loptice, zaustavljamo igricu i 
         ispisujemo poruku. */
      if(num_balls_hit == MAX_BALLS){
         animation_active = 0;
         print_text("YOU WON!", -0.15, 0.0);
      }
   }
   
   glEnable(GL_COLOR_MATERIAL);
   
   /* Postavljaju se parametri materijala. */
   glMaterialfv(GL_FRONT, GL_AMBIENT, no_material);
   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
   glMaterialfv(GL_FRONT, GL_EMISSION, no_material); 
   
   draw_ball();  
   draw_player_horns();
   draw_player_head();
   draw_player_body();

   /* Iskljucujemo svetlo da bismo mogli da crtamo strele, 
      kao i pozadinu i tajmer, iskljucujemo i materijal 
      da nam ne bi menjao boju teksta. */
   glDisable(GL_COLOR_MATERIAL);
   glDisable(GL_LIGHTING);

   if(arrow_animation){
     draw_arrow();
   }
   
   draw_background();   
   draw_timer(); 

   /* Ako je pogodjen igrac, izlazimo iz programa. */
   if(hit_flag == 1){
      exit(1);   
   }
   
   /* Postavlja se nova slika u prozor. */
   glutSwapBuffers(); 
}

static double randf(double min, double max){
    return ((double)rand()/(double)RAND_MAX) * (max-min) + min;
}

static void init_light(){
 
   /* Parametri svetla */
   GLfloat light_ambient[] = { 0, 0, 0, 1 };
   GLfloat light_diffuse[] = { 0.8, 1, 0.9, 1 };
   GLfloat light_specular[] = { 1, 1, 1, 1 };
   GLfloat model_ambient[] = { 0.4, 0.4, 0.4, 1 };
   
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

}

static void init_values(){
    
    /* Inicijalizujemo pocetne kordinate i brzine, kao i boje. */
   int i;
   for (i = 0; i < MAX_BALLS; i++) {

     xpos[i] = randf(-0.8, 0.8);
     ypos[i] = randf(-0.4, 0.7);
     vx[i] = randf(0.0, 1.0) * 0.03 + 0.01;
     vy[i] = randf(0.0, 1.0) * 0.07 + 0.05;
     red[i] = randf(0.2, 1.0);
     green[i] = 0.5;
     blue[i] = randf(0.5, 1.0);
   }
}

static void init_balls(){
  
   /* Lopticama dodeljujemo pozicije i brzine.
      Moglo je bolje da se odradi, ako stignem bice doradjeno.
      S obzirom da moraju da se pojave dve manje loptice posle
      one koja je pogodjena, ovako mi je nekako bilo najlakse 
      da ih inicijalizujem i da kasnije mogu lakse da proveravam
      pogotke. 
      0 1 2 3 4 5 6 7 8 9 10 11 12 13
      l m s s m s s l m s  s  m  s  s */

   int i, k;
   for(i = 0, k = 0; i < MAX_BALLS; i++, k++){
   
   /* Lopte sa velicinom large. */
     if(i == 0 || i == 7){
        balls[i] = new_ball(&xpos[k], &ypos[k], big_radius, 1, 'l', red[k], green[k], blue[k]);
     }
     /* Lopte sa velicinom medium */
     if(i == 1 || i == 4 || i == 8 || i == 11){
        balls[i] = new_ball(&xpos[k], &ypos[k], medium_radius, 0, 'm',  red[k], green[k], blue[k]);
     }
     /* Lopte sa velicinom small */
     if(i == 2 || i == 3 || i == 5 || i == 6 || i == 9 || i == 10 || i == 12 || i == 13){
        balls[i] = new_ball(&xpos[k], &ypos[k], small_radius, 0, 's',  red[k], green[k], blue[k]);
     }
   }

}

static void draw_arrow(){
   
   glPushMatrix();
    //glTranslatef(arrow_x, arrow_y, 0);
    glBegin(GL_LINES);
     glColor3f(0, 1, 1);
     glVertex3f(arrow_x - 0.015, arrow_y - 0.015, 0);
     glVertex3f(arrow_x, arrow_y, 0);
     glVertex3f(arrow_x + 0.015, arrow_y - 0.015, 0);
     glVertex3f(arrow_x, arrow_y, 0);
     glVertex3f(arrow_x, arrow_y - 0.15, 0);
     glVertex3f(arrow_x, arrow_y, 0);
    glEnd();
   glPopMatrix();

}

static void draw_player_horns(){
   
   /* Crtamo levi rog. */
   glPushMatrix();
    glTranslatef(player_x - 0.025, player_y + 0.072, 0);
    glRotatef(-90, 1, 0, 0);
    glRotatef(-32, 0, 1, 0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidCone(0.025, 0.03, 15, 15);
   glPopMatrix();

   /* Crtamo desni rog. */
   glPushMatrix();
    glTranslatef(player_x + 0.025, player_y + 0.072, 0);
    glRotatef(-90, 1, 0, 0);
    glRotatef(32, 0, 1, 0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidCone(0.025, 0.03, 15, 15);
   glPopMatrix();

}

static void draw_player_head(){
   
   glPushMatrix();
    glTranslatef(player_x, player_y + 0.05, 0);
    glColor3f(0.3, 0.0, 0.0);
    glutSolidSphere(0.04, 15, 15);
   glPopMatrix();

}

static void draw_player_body(){
   
   glPushMatrix();
    glTranslatef(player_x, player_y - 0.05, 0);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidCone(0.04, 0.1, 15, 15);
   glPopMatrix();

}

static void draw_ball(){

   /* Crtamo samo one lopte kojima je indikator za crtanje 1.
      Inicijalno, samo je prvoj lopti indikator 1. */
   
   int i;
   glPushMatrix();
   for (i = 0; i < MAX_BALLS; i++) {
       if(balls[i]->state == 1){
           glPushMatrix();
            glTranslatef(balls[i]->x, balls[i]->y, 0);
            glColor3f(balls[i]->red, balls[i]->green, balls[i]->blue);
            glutSolidSphere(balls[i]->r, 15, 15);
           glPopMatrix();
        }  
   }
   glPopMatrix();

}

static void draw_background(){
 
   /* Crtamo sarenu pozadinu. */
   /* TO DO: iskoristiti animation_parameter za neko fino menjanje boje. */
   glPushMatrix();
    glBegin(GL_QUADS);	
     glColor3f(0.1, 0.4, 0.3);
     glVertex2f(-1.0, -0.9);
     glColor3f(1.0, 0.2, 0.5);  
     glVertex2f(1.0, -0.9);
     glColor3f(0.1, 0.7, 1.0);
     glVertex2f(1.0, 1.0);
     glColor3f(0.4, 0.9, 0.7);
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
   
   if((*xpos - arrow_x) * (*xpos - arrow_x) + 
      (*ypos - arrow_y) * (*ypos - arrow_y) <=
      (*r) * (*r)){
	   return 1;   
   }
   else return 0;
}

static void bounce(){

   if(animation_active){
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
       ball_y_min = clip_area_y_bottom + 0.1 + balls[i]->r;
       ball_y_max = clip_area_y_top/1.2 - balls[i]->r;

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

   animation_active = 1;
  }
}

static void shoot(void){
	
   /* Povecava se y koordinata strele. */
   arrow_y += arrow_speed;
   
   if(arrow_y > clip_area_y_top){
      arrow_animation = 0;
   }
}

static Ball* new_ball(float *xbr, float *ybr, float rbr, int state, char size, float r, float g, float b){
   
   /* Kreiramo novu loptu. */
   Ball* new_ball = (Ball*)malloc(sizeof(Ball));
    
   if(new_ball == NULL){
	exit(EXIT_FAILURE);
   }
   new_ball->x = *xbr;
   new_ball->y = *ybr;
   new_ball->r = rbr;
   new_ball->state = state;
   new_ball->size = size;
   new_ball->red = r;
   new_ball->green = g;
   new_ball->blue = b;
 
   return new_ball;
}

static void print_text(char *text, float x, float y) {

   glRasterPos3f(x, y, 0);
   glutBitmapString(GLUT_BITMAP_9_BY_15, text);
}

static void print_score_value(float x, float y){
    
   char buff[10];
   snprintf(buff, 10, "%d", score_value);
   glRasterPos3f(x, y, 0);
   glutBitmapString(GLUT_BITMAP_9_BY_15, buff);
}

static void timer(int value)
{ 

   /* Azurira se vreme simulacije. */
    animation_parameter++;
  
   /* Ispaljivanje strela. */
   shoot();
   
   /* Proverava se da li je postignut pogodak.
      U slucaju da je prva loptica pogodjena njen indikator
      za crtanje postavlja se na 0, dok se za sledece 
      loptice koje treba da se crtaju postavlja na 1. */

   int p;
   for(p = 0; p < MAX_BALLS; p++){
      if(success(&balls[p]->x, &balls[p]->y, &balls[p]->r) && balls[p]->state == 1) {
                     
            if(balls[p]->size == 'l'){    // Ako je pogodjena large lopta, onda se ona vise
               balls[p]->state = 0;       // ne crta, a crtaju se dve medium loptice. 
               balls[p+1]->state = 1;      
               balls[p+4]->state = 1;
	       score_value += 100;
               num_balls_hit++;
            }                 
            if(balls[p]->size == 'm'){    // Ako je pogodjena medium lopta, onda se ona vise
               balls[p]->state = 0;       // ne crta, a crtaju se dve small loptice.
               balls[p+1]->state = 1;
               balls[p+2]->state = 1;  
 	       score_value += 150;
               num_balls_hit++;
            }
            if(balls[p]->size == 's'){     //Ako je pogodjena small lopta, dalje ne crtamo loptice.
               balls[p]->state = 0;        
               score_value += 200;         
               num_balls_hit++;
         }
                
         /* Strela prestaje da se crta onda kada pogodi lopticu. */
         arrow_animation = 0;
     }
  }
 
   /* Forsira se ponovno iscrtavanje prozora. */
   glutPostRedisplay();        
 
  /* Postavlja se ponovo tajmer po potrebi. */
   if(arrow_animation && animation_active){
      glutTimerFunc(refresh, timer, 0);
  }
	
}

static void keyboard(unsigned char key, int x, int y){
 
   switch(key){

   case 's':
   case 'r':
            /* Pokrece se igrica pritiskom na taster s, start flag 
               postavljamo na 1, da bismo znali da vise ne treba da
               ispisujemo poruku "Press s to start", a pause_flag je 
               sada postavljen na 0. 
               U slucaju tastera r, igrica se pokrece nakon sto je
               pauzirana. */ 
            if (!animation_active) {
                glutTimerFunc(10, timer, 0);
                animation_active = 1;
                start_flag = 1;
                pause_flag = 0;
            }
            break;
   case 'p':
            /* Pauzira se igrica, pause flag postavljamo na 1 da bismo znali
               da treba da ispisemo poruku "Game paused". */
             animation_active = 0;
             pause_flag = 1;
             break;
   case 'd':
            /* Kretanje u desno sve dok ne dodjemo do granice. */
            if(animation_active && player_x <= clip_area_x_right - 0.05){
            player_x += 0.03;
     	    } 
	    break;
   case 'a':
            /* Kretanje u levo sve dok ne dodjemo do granice. */
	    if(animation_active && player_x >= clip_area_x_left + 0.05){
	    player_x -= 0.03;       
	    } 
	    break;
   case 'f':
            /* Postavljamo x koordinatu strele na x koordinatu igraca. */
	    
            arrow_x = player_x;
            arrow_y = -0.65;
            if(animation_active){
          	glutTimerFunc(refresh, timer, 0);
		arrow_animation = 1;
	    }          	
            break;
   case 27:
	   /* Zavrsava se program. */
           exit(0);
           break;
   }
}

void reshape(int width, int height) {

   glViewport(0, 0, width, height);
 
   glMatrixMode(GL_PROJECTION);  
   glLoadIdentity();           

   gluOrtho2D(clip_area_x_left, clip_area_x_right, clip_area_y_bottom, clip_area_y_top);

   /* Ne dozvoljavamo promene velicine prozora. */
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
   
   /* Inicijalizuju se globalne promenljive. */
    animation_parameter = 0;
    animation_active = 0;
   
   /* Postavlja se seed za generisanje random brojeva. */
   srand(time(NULL));

   /* Inicijalizacija pocetnih koordinata i brzina loptica. */
   init_values();
 
   /* Inicijalizacija loptica. */
   init_balls();
  
   /* Ulazi se u glavnu petlju */
   glutMainLoop();              
    
   return 0;
}
