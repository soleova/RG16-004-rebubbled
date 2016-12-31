#include <GL/glut.h>  
#include <math.h>     
#include <stdio.h>
#include <time.h>
#include <GL/freeglut.h> /* Potrebno zbog funkcije glutBitmapString, da ne izbacuje upozorenje. */
#define MAX_BALL_NUMBER 14
#define REFRESH 30

typedef struct BALL{
   float x, y, r;
   int state;            /* State je indikator da li loptica treba da se crta. */ 
   char size;            /* Large, medium ili small velicina. */
   float red;  
   float green;
   float blue;
}Ball;

/* Magicna loptica je jedina loptica koja sme da pogodi igraca,
   ako igrac uspe da je pokupi, ona mu donosi dodatne poene. */ 
typedef struct MAGIC_BALL{
   float x, y, r;
   int state;
}Magic;

const char title[] = "Rebbubled";  
int window_width  = 500;   
int window_height = 500;    

/* Fleg za koliziju. */
int hit_flag;

/*Fleg za magicnu loptu. */
int magic_ball_flag;

/* Flegovi za animaciju. */
static int animation_active;
static int arrow_animation;
static int start_flag;
static int pause_flag;
static int game_over_flag;

/* Koordinate igraca, strele, "tajmera" i magicne lopte.  */
static float player_x = 0.0;
static float player_y = -0.85;
static float arrow_x = 0.0;
static float arrow_y = -0.45;
static float timer_x = 0.98;

/* Brzina strele, "tajmera" i magicne lopte. */
static float arrow_speed = 0.06; 
static float timer_speed = 0.00035;
static float magic_ball_speed = 0.005;

/* Brzine loptica. */
float vel_x[MAX_BALL_NUMBER], vel_y[MAX_BALL_NUMBER];

/* Koordinate loptica. */
float x_pos[MAX_BALL_NUMBER], y_pos[MAX_BALL_NUMBER];

/* Boje loptica. */
float red[MAX_BALL_NUMBER], green[MAX_BALL_NUMBER], blue[MAX_BALL_NUMBER];

/* Velicine precnika loptica. */
static float big_radius = 0.12;
static float medium_radius = 0.09;
static float small_radius = 0.06;
static float magic_ball_radius = 0.04;

/* Niz loptica. */
Ball* balls[MAX_BALL_NUMBER];

/* Magicna lopta. */
Magic* magic_ball;

/* Granice. */  
float ball_x_max, ball_x_min, ball_y_max, ball_y_min;
float clip_area_x_left = -1;
float clip_area_x_right = 1;
float clip_area_y_bottom = -1;
float clip_area_y_top = 1;

/* Rezultat i broj pogodjenih loptica. */
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
/* Funkcija koja pravi magicnu loptu. */
static Magic* new_magic_ball(float xbr, float ybr, float rbr, int state);
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
/* Fukcija koja crta magicnu loptu. */
static void draw_magic_ball();
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
   
   /* Inicijalizujemo osvetljenje. */
   init_light();
 
   /* Pozicija svetla. */
   GLfloat light_position[] = { 0, 1, 1, 0 };

   /* Nulti koeficijenti refleksije materijala. */
   GLfloat no_material[] = { 0, 0, 0, 1 };

   /* Koeficijenti ambijentalne refleksije materijala. */
   GLfloat material_ambient[] = { 0.7, 0.4, 0.7, 1 };

   /* Koeficijenti spekularne refleksije materijala. */
   GLfloat material_specular[] = { 0.8, 0.8, 0.8, 1 };

   /* Koeficijent spekularne refleksije za slucaj male vrednosti koeficijenta. */
   GLfloat low_shininess[] = { 15 };

   /* Pozicionira se svetlo. */
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   
   /* Da bi nam pravilno bojio tekst. */
   glDisable(GL_LIGHTING);
   
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
   /* U slucaju da je igracu isteklo vreme, ispisuje mu se rezultat. */
   else if(game_over_flag){
      
      /* Ispisivacemo ovu poruku gore na sredini gde loptice ne mogu
         da dosegnu, cisto da bi poruka bila pregledna u slucaju da imamo
         jos loptica na ekranu. */
  
      print_text("GAME OVER", -0.15, 0.8);
      print_text("YOUR SCORE: ", -0.3, 0.7);
      print_score_value(0.15, 0.7);
   
   }
   /* Kada pogodimo sve loptice, zavrsavamo igricu i ispisujemo poruku. */
   else if(num_balls_hit == MAX_BALL_NUMBER){
      
      animation_active = 0;
      print_text("YOU WON!", -0.1, 0.1);
      print_text("YOUR SCORE: ", -0.3, -0.1);
      print_score_value(0.15, -0.1);
   
   }
   /* Dok igra tece ispisujemo rezultat u gornjem levom cosku. */
   else {
     
      print_text("Score: ", -0.95, 0.9);
      print_score_value(-0.7, 0.9);

   }

   glEnable(GL_LIGHTING);
   glEnable(GL_COLOR_MATERIAL);
   
   /* Postavljaju se parametri materijala. */
   glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
   glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
   glMaterialfv(GL_FRONT, GL_EMISSION, no_material); 

   /* Crtamo loptice i igraca. */
   draw_ball();
   draw_player_horns();
   draw_player_head();
   draw_player_body();
   draw_magic_ball();
   
   /* Iskljucujemo svetlo da bismo mogli da crtamo strele, 
      kao i pozadinu i tajmer. */
   glDisable(GL_LIGHTING);

   if(arrow_animation){
     draw_arrow();
   }
   
   draw_background();   
   draw_timer();
   
   /* Postavlja se nova slika u prozor. */
   glutSwapBuffers(); 
}

static double randf(double min, double max){
    return ((double)rand()/(double)RAND_MAX) * (max-min) + min;
}

static void init_light(){
 
   /* Ambijentalna, difuzna i spekularna boja svetla. */
   GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };
   GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
   GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };
   GLfloat model_ambient[] = { 0.4, 0.4, 0.4, 1 };
   
   /* Ukljucuje se osvetljenje i podesavaju se parametri svetla. */
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
   for (i = 0; i < MAX_BALL_NUMBER; i++) {
       x_pos[i] = randf(-0.85, 0.85);
       y_pos[i] = randf(-0.3, 0.7);
       vel_x[i] = randf(0.0, 1.0) * 0.03 + 0.01;
       vel_y[i] = randf(0.0, 1.0) * 0.07 + 0.05;
       red[i] = randf(0.2, 1.0);
       green[i] = 0.5;
       blue[i] = randf(0.5, 1.0);
   }
}

static void init_balls(){
  
   /* Lopticama dodeljujemo pozicije i brzine.
      S obzirom da moraju da se pojave dve manje loptice posle
      one koja je pogodjena, ovako mi je nekako bilo najlakse 
      da ih inicijalizujem i da kasnije mogu lakse da proveravam
      pogotke. 
      0 1 2 3 4 5 6 7 8 9 10 11 12 13
      l m s s m s s l m s  s  m  s  s */

   int i, k;
   for(i = 0, k = 0; i < MAX_BALL_NUMBER; i++, k++){
   
   /* Lopte sa velicinom large. */
     if(i == 0 || i == 7){
        balls[i] = new_ball(&x_pos[k], &y_pos[k], big_radius, 1, 'l', red[k], green[k], blue[k]);
     }
     /* Lopte sa velicinom medium */
     if(i == 1 || i == 4 || i == 8 || i == 11){
        balls[i] = new_ball(&x_pos[k], &y_pos[k], medium_radius, 0, 'm',  red[k], green[k], blue[k]);
     }
     /* Lopte sa velicinom small */
     if(i == 2 || i == 3 || i == 5 || i == 6 || i == 9 || i == 10 || i == 12 || i == 13){
        balls[i] = new_ball(&x_pos[k], &y_pos[k], small_radius, 0, 's',  red[k], green[k], blue[k]);
     }
   }
}

static void draw_arrow(){
   
   /* Podebljavamo strelicu. */
   glEnable(GL_LINE_SMOOTH);
   
   glPushMatrix();
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
   
   glDisable(GL_LINE_SMOOTH);

}

static void draw_player_horns(){
   
   /* Crtamo levi rog. */
   glPushMatrix();
     glTranslatef(player_x - 0.025, player_y + 0.072, 0);
     glRotatef(-90, 1, 0, 0);
     glRotatef(-35, 0, 1, 0);
     glColor3f(0.0, 0.0, 0.0);
     glutSolidCone(0.025, 0.035, 15, 15);
   glPopMatrix();

   /* Crtamo desni rog. */
   glPushMatrix();
     glTranslatef(player_x + 0.025, player_y + 0.072, 0);
     glRotatef(-90, 1, 0, 0);
     glRotatef(35, 0, 1, 0);
     glColor3f(0.0, 0.0, 0.0);
     glutSolidCone(0.025, 0.035, 15, 15);
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
  
   for (i = 0; i < MAX_BALL_NUMBER; i++) {
       if(balls[i]->state == 1){
           glPushMatrix();
             glTranslatef(balls[i]->x, balls[i]->y, 0);
             glColor3f(balls[i]->red, balls[i]->green, balls[i]->blue);
             glutSolidSphere(balls[i]->r, 15, 15);
           glPopMatrix();
       }  
   }
}

static void draw_magic_ball(){
   
   float r = randf(0, 1);
   float g = randf(0, 1);
   float b = randf(0, 1);

   if(magic_ball->state == 1){
      glPushMatrix();
        glTranslatef(magic_ball->x, magic_ball->y, 0);
        glRotatef(-90, 1, 0, 0);
        glColor3f(r, g, b);
        /* Da bi loptica prestala da menja boje kad se igra zavrsi. */
        if(game_over_flag || num_balls_hit == MAX_BALL_NUMBER){         
           glColor3f(0.3, 0.5, 0.8);  
        }
        glutWireSphere(magic_ball->r, 15, 15);
      glPopMatrix();
   } 
}

static void draw_background(){
 
   /* Crtamo sarenu pozadinu. */
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
       glTranslatef(timer_x, 0.0, 0.0);
       glColor3f(1.0, 0.3 + timer_x, 0.2 + timer_x);
       glVertex2f(-0.98, -0.92);
       glVertex2f(timer_x, -0.92);
       glColor3f(0.0, 0.0,  0.2 + timer_x);
       glVertex2f(timer_x, -0.98);
       glVertex2f(-0.98, -0.98);
     glEnd();
   glPopMatrix();

}

static int success(float *xpos, float *ypos, float *r){
   
   /* U slucaju da je vrh strele pogodio loptu, racunamo pogodak.*/

   if(pow(*xpos - arrow_x, 2) + pow(*ypos - arrow_y, 2) <= pow(*r, 2)){
      return 1;   
   }
   else return 0;
}

static int hits(float *x, float *y, float *r){
   
   /* Na slican nacin racunamo i da li je loptica pogodila igraca.
      Ovako ce se u koliziju racunati samo bas "direktan" pogodak
      u vrh tela, ne u slucaju da ga lopta malo zakaci. */
 
   if(pow(*x - player_x, 2) + pow(*y - player_y, 2) <= pow(*r, 2)){
      return 1;   
   }
   else return 0;
}

static void bounce(){

   if(animation_active){
   int i;
   
   for(i = 0; i < MAX_BALL_NUMBER; i++){
       if(balls[i]->state == 1){
   
       /* Ubrzavamo loptice kojima je indikator
          za crtanje 1. */

       balls[i]->x += vel_x[i] * 0.03;
       balls[i]->y += vel_y[i] * 0.04;
   
       /* Granice za kretanje lopte. */
       ball_x_min = clip_area_x_left + balls[i]->r;
       ball_x_max = clip_area_x_right - balls[i]->r;
       ball_y_min = clip_area_y_bottom + 0.1 + balls[i]->r; /* + 0.1 zbog tajmera na dnu. */
       ball_y_max = clip_area_y_top/1.5 - balls[i]->r;  /* Da loptice ne bi skakale do vrha. */

       /* Ponasanje lopti u zavisnosti od toga 
          da li su udarile u zid.*/

       if(balls[i]->x > ball_x_max){
          balls[i]->x = ball_x_max;
          vel_x[i] = -vel_x[i];
       } 
       else if(balls[i]->x < ball_x_min){
          balls[i]->x = ball_x_min;
          vel_x[i]= -vel_x[i];
       }
       if(balls[i]->y > ball_y_max){
          balls[i]->y = ball_y_max;
          vel_y[i] = -vel_y[i];
       } 
       else if(balls[i]->y  < ball_y_min){
          balls[i]->y  = ball_y_min;
          vel_y[i] = -vel_y[i];
       }
   }

   /* Proverava se da li je loptica udarila igraca. */
   if(hits(&balls[i]->x, &balls[i]->y, &balls[i]->r)){
      hit_flag = 1;            
     }
   }

   /* Ako je indikator za crtanje magicne lopte 1, pustamo je
      da pada. U slucaju da je igrac ne uhvati, a ona dodje do
      tajmera, vise je ne crtamo, a kada je igrac uhvati, 
      ona nestaje i uvecava mu rezultat za 1000 poena. */

   if(magic_ball->state == 1){
      magic_ball->y -= magic_ball_speed;
      
      if(magic_ball->y < clip_area_y_bottom + 0.1 + magic_ball->r){
         magic_ball->state = 0;
      }
      if(hits(&magic_ball->x, &magic_ball->y, &magic_ball->r)){
         magic_ball->state = 0;
         score_value += 1000;
      }
   }
    
   /* Smanjujemo x koordinatu pravougaonika koji predstavlja
      tajmer. */
   timer_x -= timer_speed; 

   /* Forsira se ponovno iscrtavanje prozora. */
   glutPostRedisplay();

   animation_active = 1;
  }
   
  /* U slucaju da je tajmer istekao ili je igrac pogodjen, 
     igra se zavrsava. 
     + 0.02 zbog "ivice" pravougaonika. */
  if(timer_x < clip_area_x_left + 0.02 || hit_flag){
     animation_active = 0;
     game_over_flag = 1;
  }
}

static void shoot(void){
	
   /* Povecava se y koordinata strele. */
   arrow_y += arrow_speed;
   
   /* Kada strela dodje do granice. */
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

static Magic* new_magic_ball(float xbr, float ybr, float rbr, int state){
   
   /* Kreiramo magicnu loptu. */
   Magic* new_magic_ball = (Magic*)malloc(sizeof(Magic));
    
   if(new_magic_ball == NULL){
      exit(EXIT_FAILURE);
   }

   new_magic_ball->x = xbr;
   new_magic_ball->y = ybr;
   new_magic_ball->r = rbr;
   new_magic_ball->state = state;
   
   return new_magic_ball;
}

static void print_text(char *text, float x, float y) {
   
   /* Na prosledjenoj poziciji ispisivace nam se string. */
   glColor3f(0.0, 0.0, 0.8);
   glRasterPos3f(x, y, 0);
   glutBitmapString((void*) GLUT_BITMAP_9_BY_15, (const unsigned char*) text);

}

static void print_score_value(float x, float y){
    
   /* Rezultat prebacujemo u bafer koji cemo ispisivati. */
   char buff[10];
   snprintf(buff, 10, "%d", score_value);
   glColor3f(0.4, 0.0, 0.6);
   glRasterPos3f(x, y, 0);
   glutBitmapString((void*) GLUT_BITMAP_HELVETICA_18, (const unsigned char*) buff);

}

static void timer(int value)
{ 
  
   /* Ispaljivanje strela. */
   shoot();
   
   /* Proverava se da li je postignut pogodak.
      U slucaju da je loptica pogodjena njen indikator
      za crtanje postavlja se na 0, dok se za sledece 
      loptice koje treba da se crtaju postavlja na 1. */

   int p;
    
   for(p = 0; p < MAX_BALL_NUMBER; p++){
       if(success(&balls[p]->x, &balls[p]->y, &balls[p]->r) && balls[p]->state == 1) {
            
          if(balls[p]->size == 'l'){             // Ako je pogodjena large lopta, onda se ona vise
             balls[p]->state = 0;                // ne crta, a crtaju se dve medium loptice. 
             balls[p+1]->state = 1;      
             balls[p+4]->state = 1;
	     score_value += 100;      
             num_balls_hit++;         
          }                 

          /* Magicna loptica moze da se pojavi kada se gadjaju medium i small lopte,
             da se ne bi pojavljivala uvek, stavljena je provera parnosti indeksa i u 
             zavisnosti od njega crtace se magicna loptica sa pocetnom pozicijom istom
             kao i loptica koja upravo pogodjena. */
           
          if(balls[p]->size == 'm'){             // Ako je pogodjena medium lopta, onda se ona vise
             balls[p]->state = 0;                // ne crta, a crtaju se dve small loptice.
             balls[p+1]->state = 1;
             balls[p+2]->state = 1;  
 	     score_value += 150;
             num_balls_hit++;
  
             if(p % 2 == 0){ 
                if(magic_ball->state == 0){      // Proveravamo da li se mozda magicna lopta vec pojavila,            
                   magic_ball->state = 1;        // u tom slucaju necemo crtati novu, ali ako se trenutno 
                   magic_ball->x = balls[p]->x;  // ne iscrtava, onda cemo je crtati.
                   magic_ball->y = balls[p]->y;
                }
             }
          }
 
          if(balls[p]->size == 's'){    
             balls[p]->state = 0;        
             score_value += 200;         
             num_balls_hit++;
                
             if(p % 2 != 0){
                if(magic_ball->state == 0){       // Slicna provera za crtanje magicne loptice i ovde.        
                   magic_ball->state = 1;      
                   magic_ball->x = balls[p]->x;
                   magic_ball->y = balls[p]->y; 
                } 
             }
          }
         
          /* Strela prestaje da se crta onda kada pogodi lopticu. */
          arrow_animation = 0;
       }
   }
   
   /* Forsira se ponovno iscrtavanje prozora. */
   glutPostRedisplay();        
 
  /* Postavlja se ponovo tajmer po potrebi. */
   if(arrow_animation && animation_active){
      glutTimerFunc(REFRESH, timer, 0);
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
            /* Ova provera sprecava igraca da spamuje sa pucanjem.
               Kao i u originalnoj igrici, ako igrac promasi loptice,
               mora da saceka da strela dodje do vrha ekrana, pa tek
               onda moze opet da puca. */
  
            if(!arrow_animation){
               /* Postavljamo x koordinatu strele na x koordinatu igraca. */
               arrow_x = player_x;
               arrow_y = -0.65;
               if(animation_active){
          	  glutTimerFunc(REFRESH, timer, 0);
		  arrow_animation = 1;
	       }          	
            }
            break;
   case 27:
	   /* Izlazi se iz igrice. */
           exit(0);
           break;
   }
}

void reshape(int width, int height) {

   /* Podesava se viewport. */
   glViewport(0, 0, width, height);
   
   /* Podesava se projekcija. */
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
   glutDisplayFunc(display);    
   glutReshapeFunc(reshape);   
   glutKeyboardFunc(keyboard);
   glutIdleFunc(bounce);
   glutTimerFunc(0, timer, 0);  
   
   /* Inicijalno animacija nije aktivna. */
   animation_active = 0;
   
   /* Postavlja se seed za generisanje random brojeva. */
   srand(time(NULL));

   /* Inicijalizacija pocetnih koordinata i brzina loptica. */
   init_values();
 
   /* Inicijalizacija loptica. */
   init_balls();
  
   /* Inicijalizujemo magicnu loptu. */
   magic_ball = new_magic_ball(0, 0, magic_ball_radius, 0);
 
   /* Ulazi se u glavnu petlju */
   glutMainLoop();              
    
   return 0;
}
