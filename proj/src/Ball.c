#include "Ball.h"

Ball* create_ball(const char *team) {

Ball *ball = (Ball *) malloc(sizeof(Ball));
if( ball== NULL )
	return NULL;

ball->bmp = (Bitmap **) malloc(8*sizeof(Bitmap));
if( ball->bmp == NULL ) {
	free(ball);
	return NULL;
			}

if (!strcmp(team,"red")){
ball->bmp[0] = loadBitmap("/home/lcom/proj/src/images/red0.bmp");
ball->bmp[1] = loadBitmap("/home/lcom/proj/src/images/red1.bmp");
ball->bmp[2] = loadBitmap("/home/lcom/proj/src/images/red2.bmp");
ball->bmp[3] = loadBitmap("/home/lcom/proj/src/images/red3.bmp");
ball->bmp[4] = loadBitmap("/home/lcom/proj/src/images/red0_2.bmp");
ball->bmp[5] = loadBitmap("/home/lcom/proj/src/images/red1_2.bmp");
ball->bmp[6] = loadBitmap("/home/lcom/proj/src/images/red2_2.bmp");
ball->bmp[7] = loadBitmap("/home/lcom/proj/src/images/red3_2.bmp");
}
else if (!strcmp(team,"blue")){
ball->bmp[0] = loadBitmap("/home/lcom/proj/src/images/blue0.bmp");
ball->bmp[1] = loadBitmap("/home/lcom/proj/src/images/blue1.bmp");
ball->bmp[2] = loadBitmap("/home/lcom/proj/src/images/blue2.bmp");
ball->bmp[3] = loadBitmap("/home/lcom/proj/src/images/blue3.bmp");
ball->bmp[4] = loadBitmap("/home/lcom/proj/src/images/blue0_2.bmp");
ball->bmp[5] = loadBitmap("/home/lcom/proj/src/images/blue1_2.bmp");
ball->bmp[6] = loadBitmap("/home/lcom/proj/src/images/blue2_2.bmp");
ball->bmp[7] = loadBitmap("/home/lcom/proj/src/images/blue3_2.bmp");
}
else {
	free(ball->bmp);
	free(ball);
	return NULL;
}

ball->atual=0;
ball->x = 0;
ball->y = 0;
ball->xspeed = 0;
ball->yspeed = 0;
ball->collision = 0;
return ball;
}

void delete_ball(Ball *ball) {
if( ball == NULL )
	return;
int i;
for(i=3;i>=0;i--) 
	deleteBitmap(ball->bmp[i]);
free(ball->bmp);
free(ball);
ball = NULL;
}

int set_ball_pos(Ball *ball,short *background){
if(ball == NULL)
	return 1;

ball->x = 500;
ball->y = 475;
ball->atual = 0;
ball->collision = 0;
draw_bitmap(ball->bmp[ball->atual],(int) ball->x,(int) ball->y,3,background);
return 0;
}

int round_number(double number){
int n,sign;
if(number >=0)
	sign=1;
else 
	sign=0;

n = (int) number;
if (sign){
	number -= (int) number;
	if(number >= 0.5)
		return n+1;
	else
		return n;
}
else {
	number *= -1;
	number -= (int) number;
	if(number >= 0.5)
		return n-1;
	else
		return n;
}
}

void update_ball(Ball *ball,int xf,int yf){
ball->xf = xf;
ball->yf = yf;
int dx = xf-ball->x;
int dy = yf-ball->y;
int prints;

if(abs(dx) >= abs(dy))
	prints = abs(dx);
else
	prints = abs(dy);

ball->xspeed = 2*((float) dx / prints);
ball->yspeed = 2*((float) dy / prints);
}

void check_ball_atual(Ball *ball){
if(ball->y <= 380){
	if(ball->atual == 8) 
		ball->atual=4;
		
}
else{
	if(ball->atual == 4) 
		ball->atual=0;
}
}

int move_ball(Ball *ball, short **background) {
static int ticks=0,buffer=0;
int collision;

	ball->x += ball->xspeed;
	ball->y += ball->yspeed;
	ball->x += ball->xspeed;
	ball->y += ball->yspeed;

	if(ticks != 1){
		ticks++;
		return 1;
	}
	
	if(ball->y <= 267)
		buffer=2;
	else if(ball->y <= 380)
		buffer=1;
	else
		buffer=0;
	ball->atual++;
	check_ball_atual(ball);
	collision = draw_ball(ball->bmp[ball->atual],(int) ball->x,(int) ball->y,background[buffer],buffer);
	ticks=0;
	if(buffer == 2 && !ball->collision && ball->y-10 <= ball->yf){
		ball->collision=collision;
	}

	if((ball->y <= ball->yf)){
		ticks=0;
		return 0;
	}
	ball->x += ball->xspeed;
	ball->y += ball->yspeed;
	ball->x += ball->xspeed;
	ball->y += ball->yspeed;
	return 1;
}

int random_goal_x(){
	return rand() % 513 + 255;
}

int random_goal_y(){
	return rand() % 136 + 100;
}

int goal_or_fail(Ball *ball){
	if(ball->x >= 321 && ball->x <= 686 && ball->y >= 151 && ball->y <= 267){
		if(ball->collision)
			return 1;
		else
			return 0; //GOAL
	}
	else
		return 1; //MISS
}

void update_slope(short *slope){
	//My mouse ratio is 2/1
	slope[0] /= 6;
	slope[1] /= 12;
	slope[0] += 503;
	slope[1] += 209;

	if(slope[0] > 768)
		slope[0] = 768;
	else if(slope[0] < 255)
		slope[0] = 255;

	if(slope[1] > 267)
		slope[1] = 236;
	else if(slope[1] < 100)
		slope[1] = 100;
}

void draw_ball_buffer(Ball *ball,short *background){
	draw_bitmap(ball->bmp[ball->atual],(int) ball->x,(int) ball->y,3,background);
}

void draw_ball_waiting(Ball *ball,short *background){
	draw_bitmap(ball->bmp[ball->atual],500,475,1,background);
	ball->atual++;
	if(ball->atual == 4) 
		ball->atual=0;
}
