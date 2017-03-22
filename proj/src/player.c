#include "player.h"

Player* create_player(const char *name){
Player *player;
player = malloc(sizeof(Player));
if(player==NULL)
	return NULL;

if(!strcmp(name,"user")){
	player->stop = loadBitmap("/home/lcom/proj/src/images/j1_blue_stop.bmp");
	player->run = (Bitmap **) malloc(2*sizeof(Bitmap));
	player->run[0] = loadBitmap("/home/lcom/proj/src/images/j1_blue_right.bmp");
	player->run[1] = loadBitmap("/home/lcom/proj/src/images/j1_blue_left.bmp");
	player->shoot = loadBitmap("/home/lcom/proj/src/images/j1_blue_shoot.bmp");
	player->goal = (Bitmap **) malloc(2*sizeof(Bitmap));
	player->goal[0] = loadBitmap("/home/lcom/proj/src/images/blue_goal0.bmp");
	player->goal[1] = loadBitmap("/home/lcom/proj/src/images/blue_goal1.bmp");
	player->ball = create_ball("blue");
}
else if(!strcmp(name,"com")){
	player->stop = loadBitmap("/home/lcom/proj/src/images/j2_red_stop.bmp");
	player->run = (Bitmap **) malloc(2*sizeof(Bitmap));
	player->run[0] = loadBitmap("/home/lcom/proj/src/images/j2_red_right.bmp");
	player->run[1] = loadBitmap("/home/lcom/proj/src/images/j2_red_left.bmp");
	player->shoot = loadBitmap("/home/lcom/proj/src/images/j2_red_shoot.bmp"); 
	player->goal = (Bitmap **) malloc(2*sizeof(Bitmap));
	player->goal[0] = loadBitmap("/home/lcom/proj/src/images/red_goal0.bmp");
	player->goal[1] = loadBitmap("/home/lcom/proj/src/images/red_goal1.bmp"); 
	player->ball = create_ball("red");
}
else{
	free(player);
	return NULL;
}

player->goals=0;
player->attempts=0;
return player;
}

void delete_player(Player *player){
int i;
delete_ball(player->ball);
deleteBitmap(player->shoot);
for(i=1;i>=0;i--)
	deleteBitmap(player->run[i]);
free(player->run);
for(i=1;i>=0;i--)
	deleteBitmap(player->goal[i]);
free(player->goal);
deleteBitmap(player->stop);
free(player);
player = NULL;
}

void set_player_stop(Player *player, short *background){
player->x = 242;
player->y = 560;
player->atual = 0;
draw_bitmap(player->stop,player->x,player->y,2,background);
}

void set_player_shoot(Player *player, short **background){
player->x = 450;
player->y = 352;
player->atual = 0;
draw_bitmap(player->shoot,player->x,player->y,3,background[1]);
draw_bitmap(player->shoot,player->x,player->y,3,background[2]);
draw_bitmap(player->shoot,player->x,player->y,2,background[0]);
}

int move_player(Player *player, short *background){
static int ticks=0;
	if(player->x == 450 && player->y == 352){
		ticks=0;
		return 0;
	}

	if(ticks != 17){
		ticks++;
		player->x += 2;
		player->y -= 2;
		draw_bitmap(player->run[player->atual],player->x,player->y,1,background);
		return 1;
	}
	
	player->atual++;
	if(player->atual == 2) 
		player->atual=0;
	draw_bitmap(player->run[player->atual],player->x,player->y,1,background);
	ticks=0;
	return 1;
}

int reaction_player(Player *player,short *background,int goal){
static int time=0,frame=0;
	if(time % 20 != 0){
		time++;
		return 1;
	}
	if(!goal){
		draw_bitmap(player->goal[frame],player->x,player->y,1,background);
		frame++;
		if(frame==2)
			frame=0;
		}
	else {}
	time++;
	return 0;
}
