#include "goalkeeper.h"

GoalKeeper* create_goalkeeper(){
GoalKeeper *gk;
gk = malloc(sizeof(GoalKeeper));
if(gk==NULL)
	return NULL;

gk->x = 485;
gk->y = 164;
gk->keys = (int *) calloc(4,sizeof(int));
gk->key_n = 0;
gk->save_area = 0;
gk->area = (Bitmap **) malloc(2*sizeof(Bitmap));
gk->area[0] = loadBitmap("/home/lcom/proj/src/images/area2.bmp");
gk->area[1] = loadBitmap("/home/lcom/proj/src/images/area.bmp");
gk->save = (Bitmap **) malloc(9*sizeof(Bitmap));
gk->save[0] = loadBitmap("/home/lcom/proj/src/images/gk_stop.bmp");
gk->save[1] = loadBitmap("/home/lcom/proj/src/images/gk_left.bmp");
gk->save[2] = loadBitmap("/home/lcom/proj/src/images/gk_up.bmp");
gk->save[3] = loadBitmap("/home/lcom/proj/src/images/gk_right.bmp");
gk->save[4] = loadBitmap("/home/lcom/proj/src/images/gk_down_left.bmp");
gk->save[5] = loadBitmap("/home/lcom/proj/src/images/gk_down.bmp");
gk->save[6] = loadBitmap("/home/lcom/proj/src/images/gk_down_right.bmp");
gk->save[7] = loadBitmap("/home/lcom/proj/src/images/gk_st_left.bmp");
gk->save[8] = loadBitmap("/home/lcom/proj/src/images/gk_st_right.bmp");
return gk;
}

void delete_goalkeeper(GoalKeeper *gk){
int i;
for(i=7;i>=0;i--)
	deleteBitmap(gk->save[i]);
for(i=1;i>=0;i--)
	deleteBitmap(gk->area[i]);
free(gk->keys);
free(gk->save);
free(gk->area);
free(gk);
gk = NULL;
}

void print_area(GoalKeeper *gk,short *background){
switch(gk->save_area){
	case 1:
		draw_bitmap(gk->area[1],321,151,4,background);
		break;
	case 2:
		draw_bitmap(gk->area[1],448,151,4,background);
		break;
	case 3:
		draw_bitmap(gk->area[1],576,151,4,background);
		break;
	case 4:
		draw_bitmap(gk->area[1],321,219,4,background);
		break;
	case 5:
		draw_bitmap(gk->area[1],448,219,4,background);
		break;
	case 6:
		draw_bitmap(gk->area[1],576,219,4,background);
		break;
	case 7:
		draw_bitmap(gk->area[0],321,151,4,background);
		break;
	case 8:
		draw_bitmap(gk->area[0],576,151,4,background);
		break;
	case 0:
		draw_bitmap(gk->area[0],448,151,4,background);
		break;
}
}

int choose_area(GoalKeeper *gk){
static unsigned long key=0,final=0;
static int i=0;
key = ler_tecla();

if (key == ESC)
	return -1;
if(gk->key_n == 2)
	return 1;

	if(key==TWO_BYTES && !i){
		final = 0;
		final = key<<8;
		i++;
		return 1;
	}
	else{
		i=0;
		final |= key;						
	}

switch(final){
	case UP_ARROW:
		gk->keys[0] = 1;
		gk->key_n++;
		break;
	case DOWN_ARROW:
		gk->keys[1] = 1;
		gk->key_n++;
		break;
	case LEFT_ARROW:
		gk->keys[2] = 1;
		gk->key_n++;
		break;
	case RIGHT_ARROW:
		gk->keys[3] = 1;
		gk->key_n++;
		break;
	case ESC:
		final = 0;
		return -1;
	default:
		final = 0;
		return 1;
}
final = 0;
if(gk->key_n >= 0)
	return 0;
}

void area(GoalKeeper *gk){
switch(gk->key_n){
	case 0:
		gk->save_area = 0;
	case 1:
		if(gk->keys[0])
			gk->save_area = 2;
		else if(gk->keys[1])
			gk->save_area = 5;
		else if(gk->keys[2])
			gk->save_area = 7;
		else if(gk->keys[3])
			gk->save_area = 8;
		else
			gk->save_area = 0;
		break;
	case 2:
		if(gk->keys[0] && gk->keys[2])
			gk->save_area = 1;
		else if(gk->keys[0] && gk->keys[3])
			gk->save_area = 3;
		else if(gk->keys[1] && gk->keys[2])
			gk->save_area = 4;
		else if(gk->keys[1] && gk->keys[3])
			gk->save_area = 6;
		else if(gk->keys[0] && gk->keys[1])
			gk->save_area = 0;
		else
			gk->save_area = 0;
		break;
}
}

void random_area(GoalKeeper *gk){
gk->save_area = rand() % 8;
}

void reset_gk(GoalKeeper *gk){
int i;
for(i=0;i<3;i++)
	gk->keys[i] = 0;
gk->key_n = 0;
gk->save_area = 0;
}

void update_goalkeeper(GoalKeeper *gk,short **background,short *reaction){
switch(gk->save_area){
	case 0:
		draw_bitmap(gk->save[0],gk->x,gk->y,3,background[1]);
		draw_bitmap(gk->save[0],gk->x,gk->y,3,background[2]);
		break;
	case 1:
		draw_bitmap(gk->save[7],412,181,3,background[1]);
		draw_bitmap(gk->save[1],321,157,3,background[2]);
		break;
	case 2:
		draw_bitmap(gk->save[2],485,158,3,background[1]);
		draw_bitmap(gk->save[2],485,158,3,background[2]);
		break;
	case 3:
		draw_bitmap(gk->save[8],526,181,3,background[1]);
		draw_bitmap(gk->save[3],561,157,3,background[2]);
		break;
	case 4:
		draw_bitmap(gk->save[7],412,181,3,background[1]);
		draw_bitmap(gk->save[4],323,243,3,background[2]);
		break;
	case 5:
		draw_bitmap(gk->save[5],488,192,3,background[1]);
		draw_bitmap(gk->save[5],488,192,3,background[2]);
		break;
	case 6:
		draw_bitmap(gk->save[8],526,181,3,background[1]);
		draw_bitmap(gk->save[6],555,243,3,background[2]);
		break;
	case 7:
		draw_bitmap(gk->save[7],412,181,3,background[1]);
		draw_bitmap(gk->save[1],321,187,3,background[2]);
		break;
	case 8:
		draw_bitmap(gk->save[8],526,181,3,background[1]);
		draw_bitmap(gk->save[3],561,187,3,background[2]);
		break;
}
	memcpy((char*)reaction,background[2],HRES*VRES*2);
}

