// Jack Rellinger
// project.c
// This program uses x11 graphics to create a version of the mobile game "aa", where arrows are
// shot by a rotating shooter onto a central ball, and the player tries not to let any of them
// touch.
//
// WARNING: May break if shots are fired too quickly. But you will also lose if shots are fired
// too quickly. So it works out most of the time.

#include <stdio.h>
#include "gfx2.h"
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

// struct to store the placed shot lines
typedef struct{
	float x1;
	float x2;
	float y1;
	float y2;
} PlacedShot;

// function prototypes
void loadInScreen(int wd, int ht, char c);
int shoot(float xe, float ye, float xs, float ys, int xc, int yc, int circ_r, int tip_r, int r,float theta, PlacedShot placed_arr[], int count);

int addShot(PlacedShot placed_arr[], int count, int circ_r, int r, float theta, int xc, int yc);
void dispPlacedShots(PlacedShot placed_arr[], int tip_r, int count);
bool checkOverlap(PlacedShot placed_arr[], int count, int tip_r, int wd, int ht);

void gameOver(int count, int wd, int ht);


int main() 
{
	// declar variables and open window
	int wd = 800, ht = 800;
	gfx_open(wd, ht, "gx");

	int xc = wd/2, yc = ht/2; // center of window
	int r = 6*wd/16; // rotation radius
	
	float dtheta = M_PI/400; // rotation speed
	float theta = 2*M_PI/2; // start at bottom
	float xe = xc + r*cos(theta), ye = r*sin(theta); // end of rotating shooter coords
	float xs = xc + 3*r/4*cos(theta), ys = 3*r/4*sin(theta); // start of rotating shooter coords
	
	int circ_r = wd/9; // middle circle radius
	int tip_r = 7; // tip of arrow radius

	int count = 0; // counts the placed shots
	PlacedShot placed_arr[100]; // array to store placed shots
		
	char c;
	
	char counter[4];
	
	loadInScreen(wd, ht, c);
	
	while(1){
		gfx_clear();
		gfx_line(xs, ys, xe, ye);
		gfx_fill_circle(xc, yc, circ_r);
		gfx_fill_circle(xe, ye, tip_r);
		dispPlacedShots(placed_arr, tip_r, count);
		
		gfx_text(40, 40, "Hits:"); 
		sprintf(counter, "%d", count);
		gfx_text(40, 60, counter);
		
		theta += dtheta;
		
		// adjust shooter location
		xe = xc + r*cos(theta);
		ye = yc + r*sin(theta);
		xs = xc + 3*r/4*cos(theta);
		ys = yc + 3*r/4*sin(theta);

		gfx_flush();
		usleep(10000);
		
		if (gfx_event_waiting()){
			c = gfx_wait();
			if (c == 'q') break;
			if (c == 32){ 
				count = shoot(xe, ye, xs, ys, xc, yc, circ_r, tip_r, r, theta, placed_arr, count);	
				if (checkOverlap(placed_arr, count, tip_r, wd, ht)) break;
			}
		}
	}
	
	return 0;
}

// displays the starting screen text
void loadInScreen(int wd, int ht, char c)
{
	while(1){
		gfx_text(wd/4, ht/4, "Welcome to \"gx\", an accuracy strategy game.");
		gfx_text(wd/4, ht/4 + 20, "Directions: "); 
		gfx_text(wd/4, ht/4 + 40, "   Try and fit as many arrows on the ball as possible without touching each other."); 
		gfx_text(wd/4, ht/4 + 60, "Controls:");
		gfx_text(wd/4, ht/4 + 80, "   space -- shoot arrow onto ball ");
		gfx_text(wd/4, ht/4 + 100, "   q -- quit");
		gfx_text(wd/4, ht/4 + 140, "Press space to continue to game");
		
		if(gfx_event_waiting()){
			c = gfx_wait();
			if (c == 32) break;
		}
	}

}

// shoots the arrows onto the ball
int shoot(float xe, float ye, float xs, float ys, int xc, int yc, int circ_r, int tip_r, int r, float theta, PlacedShot placed_arr[], int count)
{
	// initialize the locations of the animated shot
	float xshot = xs, yshot = ys, xshotEnd = xe, yshotEnd = ye;
	// calculate the shot trajectory and speed
	int dx = .05*r*cos(theta), dy = .05*r*sin(theta);	
	while(1){
		gfx_clear();
		// display shot as it moves
		gfx_line(xshot, yshot, xshotEnd, yshotEnd);
		gfx_fill_circle(xshotEnd, yshotEnd, tip_r);
		
		// display rest of board as shot moves
		gfx_line(xs, ys, xe, ye);
		gfx_fill_circle(xe, ye, tip_r);
		gfx_fill_circle(xc, yc, circ_r);
		dispPlacedShots(placed_arr, tip_r, count);

		// animate shot
		xshotEnd = xshotEnd - dx;
		yshotEnd = yshotEnd - dy;
		xshot = xshot - dx;
		yshot = yshot - dy;
		
		// check if shot hits circle(different depending on quadrant, then add to arr of structs
		if(dx > 0 && dy > 0){
			if(xshot < xc+circ_r*cos(theta) && yshot < yc+circ_r*sin(theta)){
				addShot(placed_arr, count, circ_r, r, theta, xc, yc);
				count++;	
				break;
			}	
		}else if(dx < 0 && dy > 0){
			if(xshot > xc+circ_r*cos(theta) && yshot < yc+circ_r*sin(theta)){
				addShot(placed_arr, count, circ_r, r, theta, xc, yc);
				count++;
				break;
			}
		}else if(dx > 0 && dy < 0){
			if(xshot < xc+circ_r*cos(theta) && yshot > yc+circ_r*sin(theta)){
				addShot(placed_arr, count, circ_r, r, theta, xc, yc);
				count++;
				break;
			}
		}else if(dx < 0 && dy < 0){
			if(xshot > xc+circ_r*cos(theta) && yshot > yc+circ_r*sin(theta)){
				addShot(placed_arr, count, circ_r, r, theta, xc, yc);
				count++;
				break;
			}
		}else if(dx == 0){
			if(dy > 0){
				if(yshot < yc+circ_r*sin(theta)){
					addShot(placed_arr, count, circ_r, r, theta, xc, yc);
					count++;
					break;
				}
			}else if(dy < 0){ 
				if(yshot > yc+circ_r*sin(theta)){
					addShot(placed_arr, count, circ_r, r, theta, xc, yc);
					count++;
					break;
				}
			}
		}else if(dy == 0){
			if(dx > 0){
				if(xshot < xc+circ_r*cos(theta)){
					addShot(placed_arr, count, circ_r, r, theta, xc, yc);
					count++;
					break;
				}
			}else if(dx < 0){ 
				if(xshot > xc+circ_r*cos(theta)){
					addShot(placed_arr, count, circ_r, r, theta, xc, yc);
					count++;
					break;
				}
			}
		}	
		
	
		gfx_flush();
		usleep(10000);
		
	}
	return count;
}

// adds a shot to the placed shots array
int addShot(PlacedShot placed_arr[], int count, int circ_r, int r, float theta, int xc, int yc)
{
	placed_arr[count].x1 = xc+circ_r*cos(theta);
	placed_arr[count].y1 = xc+circ_r*sin(theta);
	placed_arr[count].x2 = placed_arr[count].x1 + r/4*cos(theta);
	placed_arr[count].y2 = placed_arr[count].y1 + r/4*sin(theta);
	
}

// display all placed shots in the array
void dispPlacedShots(PlacedShot placed_arr[], int tip_r, int count)
{
	for (int i = 0; i < count; i++){
		gfx_line(placed_arr[i].x1, placed_arr[i].y1, placed_arr[i].x2, placed_arr[i].y2);
		gfx_fill_circle(placed_arr[i].x2, placed_arr[i].y2, tip_r);
	}
}

// checks if the tips of the arrows overlap
bool checkOverlap(PlacedShot placed_arr[], int count, int tip_r, int wd, int ht)
{
	bool overlap = false;
	float distance;
	if (count > 1){
		for (int i = 0; i < count -1; i++){
			distance = sqrt(pow(placed_arr[i].x2 - placed_arr[count-1].x2, 2) + pow(placed_arr[i].y2 - placed_arr[count-1].y2, 2));
			if (distance < tip_r * 2) {
				gameOver(count, wd, ht);
				overlap = true;
				break;
			}
			
		}
	}
	return overlap;
}

// displays the game over screen
void gameOver(int count, int wd, int ht)
{
	char score[4];
	char c;
	sprintf(score, "%d", count);

	while(1){
		gfx_clear();
		
		gfx_text(wd/4, ht/4, "GAME OVER");
		gfx_text(wd/4, ht/4 + 40, "FINAL SCORE:");
		gfx_text(wd/4, ht/4 + 60, score);
		gfx_text(wd/4, ht/4 + 100, "Press q to quit.");
		
		if(gfx_event_waiting()){
			c = gfx_wait();
			if (c == 'q') break;
		}
		
		gfx_flush();
		usleep(10000);
	}
}


