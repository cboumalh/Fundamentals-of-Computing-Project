// Christopher Boumalhab


#include "gfx.h"
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#define MAXENEM 100

typedef struct{ // struct for the invaders 
	
	int enemyCoordinates[MAXENEM][2];
	int enemySpeed;
	int enemyDelay;
	int enemyRadius;
	int enemyKilled;

}enemy_s;


void setBase(int x, int y); // function to set the base of the cannon
void drawCannon(int x, int y, int xpress, int ypress); // function to draw cannon
void shootCannon(int x, int y, int xpress, int ypress, enemy_s *attacker, time_t start, char score[]); // function to shoot bullet and check if it hits something
void drawExplosion(int x, int y); // function to draw explosion
void testCollision(int x, int y, enemy_s *attacker); // function to test if bullet and invader collided
int  randPos(); // rand function specific to this game


int main(){

srand(time(0)); 
const int width = 600, height = 600; // height and width of screen
time_t start = time(NULL); // time the program starts running
enemy_s attacker; // struct for the invaders


gfx_open(width, height, "project");
int xCannon = width/2, yCannon = height - 80; // coordinates of the tip of the cannon initially (vertical upwards)

int k = 1; // counter to make sure a new invader comes every k * enemyDelay seconds
int i; // counter to loop through array of invaders
int j = 0; // counter to subtract time delay every time a bullet is shot

for(i = 1; i < 100; i++){ // initializing every invader's coordinates starting from the second one  to x = -1 and y = 0 
	attacker.enemyCoordinates[i][0] = -1;
	attacker.enemyCoordinates[i][1] = 0;
}

// initial conidtions for first invader
attacker.enemyCoordinates[0][0] = randPos();
attacker.enemyCoordinates[0][1] = 0;

//conditions for all the invaders
attacker.enemySpeed = 1;
attacker.enemyDelay = 6;
attacker.enemyRadius = 10;
attacker.enemyKilled = 0;

	while(1){

		gfx_clear();
		usleep(150000);
		i = 0; // counter described above

		char score[15];
		sprintf(score, "SCORE: %d", attacker.enemyKilled);
		gfx_text(5, height - 35, score); // printing the number of dead invaders

 		while(attacker.enemyCoordinates[i][0] != -1 && i < MAXENEM){ // function to display the invaders 
			gfx_circle(attacker.enemyCoordinates[i][0], attacker.enemyCoordinates[i][1], attacker.enemyRadius);
			attacker.enemyCoordinates[i][1] += attacker.enemySpeed;
			if(attacker.enemyCoordinates[i][1] >= height - 15 && attacker.enemyCoordinates[i][0] !=  -11) { // condition to check no invader hit the cannon base
				while(1){	
				gfx_flush();
				setBase(width, height);
				gfx_text(width/2 - 30, 3.0/4 * height, "YOU LOSE");
				} 
			}
			i++;
		}
	
		if((int)(time(NULL) - start) - j*4 >= k * attacker.enemyDelay){ // checking if it's time to add a new invader
			 attacker.enemyCoordinates[i][0] = randPos();
			 k++;
			}

    // drawing base of cannon and the cannon
		setBase(width, height); 
		drawCannon(width, height, xCannon, yCannon);

		if(gfx_event_waiting() == 1 && gfx_wait() == 1){ // if left mouse button pressed to shoot
			xCannon = gfx_xpos();
			yCannon = gfx_ypos();
			drawCannon(width, height, xCannon, yCannon); // draw the new cannon angled the same way as where i pressed
			shootCannon(width, height, xCannon, yCannon, &attacker, start, score); // shoot cannon and check if target hit
			j++;
			}
			
	}
}



void setBase(int x, int y){
	int i = 0;
	int k = 0;

	for(i = 0; i < x; i = i + 5){
		if(i >= x/2 - 30 && i <= x/2 + 30){
			for(k = 0; k <= 30 ; k = k + 5)
		    gfx_text(i, y - k, "#");
			}	
		else{
			for(k = 0; k <= 15; k = k + 5)
				gfx_text(i, y - k, "^");
			}
	}

}


void drawCannon(int x, int y, int xpress, int ypress){
	int cannonLength = 50;	
	float theta = (float) atan2((double)(y - ypress - 30.0), (double)(xpress - x/2.0));
	gfx_line(x/2, y - 40, x/2 + round(cannonLength*cos(theta)), y - 30 - round(cannonLength*sin(theta))); // trigonometry to check direction of cannon aim
	
}


void shootCannon(int x, int y, int xpress, int ypress, enemy_s *attacker, time_t start, char score[]){

	int cannonLength = 50; 
	float theta = (float) atan2((double)(y - ypress - 30.0), (double)(xpress - x/2.0));
	int cannonTipX = x/2 + round(cannonLength*cos(theta));
	int cannonTipY = y - 30 - round(cannonLength*sin(theta));
	
	if(theta < 1.57){ //checking whether the bullet is fired in the first quadrant to know whether xBullet will increase or decrease
		int i = 0, k;
		while(cannonTipX + i < x && cannonTipY - i*tan((double)theta) > 0){ // while bullet has not hit the boundaries
		  k = 0;
			setBase(x,y);	
			drawCannon(x, y, xpress, ypress);
			gfx_text(5, y - 35, score); //score of how many invaders killed

			while((*attacker).enemyCoordinates[k][0] != -1 && k < MAXENEM){ // draw all the invaders that have been initialized and increment their y position
				gfx_circle((*attacker).enemyCoordinates[k][0], (*attacker).enemyCoordinates[k][1], (*attacker).enemyRadius);
				(*attacker).enemyCoordinates[k][1] += (*attacker).enemySpeed;
				k++;
			}
	
			gfx_text(cannonTipX + i,(int)(cannonTipY - i*tan((double)theta)), "*"); // draw bullet at a position along its path
			gfx_text(x/2 - 30, y/2, "RELOADING"); // cannon reloads while bullet is flying

			testCollision(cannonTipX + i, (int)(cannonTipY - i*tan((double)theta)), attacker); // function that tests whether bullet has hit a invader at that position
	
			gfx_flush();
			gfx_clear();

			i+=7;// speed of the bullet is 7
			usleep(150000);
		}
			(*attacker).enemyCoordinates[k][0] = randPos(); // coordinates of a new invader

	}
	else{
		int i = 0, k;
		while(cannonTipX - i > 0 && cannonTipY + i*tan((double)theta) > 0){	// same condition as above but for second quadrant
		  k = 0;
			setBase(x,y);
			drawCannon(x, y, xpress, ypress);
			gfx_text(5, y - 35, score);

			while((*attacker).enemyCoordinates[k][0] != -1 && k < MAXENEM){
				gfx_circle((*attacker).enemyCoordinates[k][0], (*attacker).enemyCoordinates[k][1], (*attacker).enemyRadius);
				(*attacker).enemyCoordinates[k][1] += (*attacker).enemySpeed;
				k++;
			}
	
			gfx_text(cannonTipX - i,(int)(cannonTipY + i*tan((double)theta)), "*");
			gfx_text(x/2 - 30, y/2, "RELOADING");
			
	    testCollision(cannonTipX - i, (int)(cannonTipY + i*tan((double)theta)), attacker); 
 
			gfx_flush();
			gfx_clear();

			i+=7;
			usleep(150000);
		}
	
			(*attacker).enemyCoordinates[k][0] = randPos();

	}
}


void drawExplosion(int x, int y){ // draws a star as an explosion
	int i = 13;

		gfx_line(x + i , y + i, x - i , y - i);
		gfx_line(x + i , y - i, x - i , y + i);
		gfx_line(x - i, y, x + i, y);

		usleep(200000);
}


float calcDistance(int x, int y, int x1, int y1){ // distance formula to check whether bullet hit the invader
	return sqrt((float)(x1-x)*(x1-x) + (y1-y)*(y1-y));
}


void testCollision(int x, int y, enemy_s *attacker){ // uses distance formula for bullet and every invader on the screen
	int k = 0;
	while((*attacker).enemyCoordinates[k][0] != -1 && k < MAXENEM){
			if(calcDistance(x, y, (*attacker).enemyCoordinates[k][0],(*attacker).enemyCoordinates[k][1]) < 13){
				drawExplosion(x, y);
				// placing killed invaders outside of the screen 
				(*attacker).enemyCoordinates[k][0] = -11; 
				(*attacker).enemyCoordinates[k][1] = -11;
				(*attacker).enemyKilled++;
			}			
	k++;
	}
}


int randPos(){ // random function that places invaders near the two ends of the screen because bullet moves to fast in the middle 
	int k = rand()%2;

	if(k == 0) return rand()%231;
	else return 370 + (rand()%231);
}
