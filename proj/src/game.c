#include "game.h"

Game* create_game(){
Game *game;
int i;
game = malloc(sizeof(Game));
if(game == NULL) 
	return NULL;

game->wallpapper = loadBitmap("/home/lcom/proj/src/images/game_wall.bmp");
if(game->wallpapper == NULL){
	free(game);
	return NULL;
}

game->background = calloc(HRES*VRES*2,sizeof(short));
game->background_stop = calloc(HRES*VRES*2,sizeof(short));
game->background_run = calloc(HRES*VRES*2,sizeof(short));

game->background_shoot = (short **) calloc(3,sizeof(short*));
for(i=0;i<3;i++)
	game->background_shoot[i] = (short *) calloc(HRES*VRES*2,sizeof(short));
game->background_reaction = calloc(HRES*VRES*2,sizeof(short));
if(game->background == NULL || game->background_stop == NULL || game->background_run == NULL || game->background_shoot == NULL || game->background_reaction == NULL){
	deleteBitmap(game->wallpapper);
	free(game);
	return NULL;
}

game->score = (Bitmap **) malloc(2*sizeof(Bitmap));
if(game->score == NULL){
	free(game->background);
	deleteBitmap(game->wallpapper);
	free(game);
	return NULL;
}
game->score[0] = loadBitmap("/home/lcom/proj/src/images/goal.bmp");
game->score[1] = loadBitmap("/home/lcom/proj/src/images/fail.bmp");

game->numbers = loadBitmap("/home/lcom/proj/src/images/numbers.bmp");

game->gameover = (Bitmap **) malloc(4*sizeof(Bitmap));
game->gameover[0] = loadBitmap("/home/lcom/proj/src/images/user_small.bmp");
game->gameover[1] = loadBitmap("/home/lcom/proj/src/images/user_big.bmp");
game->gameover[2] = loadBitmap("/home/lcom/proj/src/images/com_small.bmp");
game->gameover[3] = loadBitmap("/home/lcom/proj/src/images/com_big.bmp");

game->referee = (Bitmap **) malloc(2*sizeof(Bitmap));

game->referee[0] = loadBitmap("/home/lcom/proj/src/images/ref0.bmp");
game->referee[1] = loadBitmap("/home/lcom/proj/src/images/ref1.bmp");

game->mse = create_mouse("game");
game->slope = (short *) malloc(2*sizeof(short));
game->slope[0] = 0;
game->slope[1] = 0;
game->user = create_player("user");
game->com = create_player("com");
game->gk = create_goalkeeper();
game->penalty = USER;
game->mode = NORMAL;

return game;
}

void delete_game(Game *game){
int i;
delete_mouse(game->mse);
free(game->slope);
delete_goalkeeper(game->gk);
delete_player(game->user);
delete_player(game->com);
for(i=1;i>=0;i--)
	deleteBitmap(game->score[i]);
free(game->score);
for(i=3;i>=0;i--)
	deleteBitmap(game->gameover[i]);
free(game->gameover);
for(i=1;i>=0;i--)
	deleteBitmap(game->referee[i]);
free(game->referee);
deleteBitmap(game->numbers);
free(game->background);
free(game->background_stop);
free(game->background_run);
for(i=2;i>=0;i--)
	free(game->background_shoot[i]);
free(game->background_shoot);
free(game->background_reaction);
deleteBitmap(game->wallpapper);
free(game);
game = NULL;
}

void whistle_referee(Game *game){
	draw_bitmap(game->referee[1],0,302,2,game->background_stop);
}

void draw_gamewall(Game *game,int state){
static int i=0;
if(!state)
	draw_bitmap(game->wallpapper,0,0,0,game->background);
else
	draw_buffer(game->background);

//GOALS
drawBitmap_number(game->numbers,"user",game->user->goals,game->background);
drawBitmap_number(game->numbers,"com",game->com->goals,game->background);
if(state==2){
	switch(game->penalty){
		case GAMEOVER_USER:
			if(i%2==0)
				draw_bitmap(game->gameover[1],233,328,2,game->background);
			else
				draw_bitmap(game->gameover[0],233,328,2,game->background);
			i++;
			return;
		case GAMEOVER_COM:
			if(i%2==0)
				draw_bitmap(game->gameover[3],236,334,2,game->background);
			else
				draw_bitmap(game->gameover[2],236,334,2,game->background);
			i++;
			return;
		default:
			return;
	}
}
	
//BUFFERS
memcpy((char*)game->background_stop,game->background,HRES*VRES*2);
memcpy((char*)game->background_run,game->background,HRES*VRES*2);
memcpy((char*)game->background_shoot[0],game->background,HRES*VRES*2);
memcpy((char*)game->background_shoot[1],game->background,HRES*VRES*2);
memcpy((char*)game->background_shoot[2],game->background,HRES*VRES*2);
memcpy((char*)game->background_reaction,game->background,HRES*VRES*2);
//REFEREE
draw_bitmap(game->referee[0],0,302,3,game->background_stop);
draw_bitmap(game->referee[1],0,302,3,game->background_run);
draw_bitmap(game->referee[1],0,302,3,game->background_shoot[0]);
draw_bitmap(game->referee[1],0,302,3,game->background_shoot[1]);
draw_bitmap(game->referee[1],0,302,3,game->background_shoot[2]);
draw_bitmap(game->referee[1],0,302,3,game->background_reaction);
//GK
draw_bitmap(game->gk->save[0],game->gk->x,game->gk->y,3,game->background_stop);
draw_bitmap(game->gk->save[0],game->gk->x,game->gk->y,3,game->background_run);
draw_bitmap(game->gk->save[0],game->gk->x,game->gk->y,3,game->background_shoot[0]);
i=0;
}

void reset_gamewall(Game *game){
	draw_gamewall(game,0);
	game->user->attempts = 0;
	game->com->attempts = 0;
}

void update_game(Game *game){
static int i=0;

if(!game->user->attempts)
	i=0;

//NEXT PENALTY
if(i%2 == 0){
	if(goal_or_fail(game->user->ball))
		draw_bitmap(game->score[1],320+28*game->user->attempts,66,2,game->background);
	else{
		draw_bitmap(game->score[0],320+28*game->user->attempts,66,2,game->background);
		game->user->goals++;
	}
	game->user->attempts++;
	game->penalty = COM;
	reset_gk(game->gk);
}
else{
	if(goal_or_fail(game->com->ball))
		draw_bitmap(game->score[1],570+28*game->com->attempts,66,2,game->background);
	else{
		draw_bitmap(game->score[0],570+28*game->com->attempts,66,2,game->background);
		game->com->goals++;
	}
	game->com->attempts++;
	game->penalty = USER;
	reset_gk(game->gk);
}
i++;
draw_gamewall(game,1);
//LOGIC
switch(game->mode){
	case NORMAL: 
		if( (game->user->goals > ( 5 - game->com->attempts + game->com->goals)) ){
			game->penalty = GAMEOVER_USER;
			i=0;
		}
		else if( (game->com->goals > ( 5 - game->user->attempts + game->user->goals)) ){
			game->penalty = GAMEOVER_COM;
			i=0;
		}
		else if( (game->user->goals == game->com->goals) && (game->user->attempts == game->com->attempts) && (game->user->attempts == 5)){
			game->mode = SUDDEN_DEATH;
			reset_gamewall(game);
		}
		break;
	
	case SUDDEN_DEATH:
		if( (game->user->goals > game->com->goals) && (game->user->attempts == game->com->attempts) ){
			game->penalty = GAMEOVER_USER;
			i=0;
		}
		else if( (game->user->goals < game->com->goals) && (game->user->attempts == game->com->attempts) ){
			game->penalty = GAMEOVER_COM;
			i=0;
		}
		else if(game->com->attempts == 5)
			reset_gamewall(game);
		break;
}

}

int start_game(){

Game *game;
game = create_game();
if(game == NULL) 
	return 1;
draw_gamewall(game,0);

int ipc_status;
int r;
int on=1;
unsigned long key=0;
message msg;
int time=0,i=0,goal=0;

while (key != ESC) {
		
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				 if (msg.NOTIFY_ARG & IRQ_KBD_HOOK){
					switch(game->penalty){
						case WAIT_SHOOT_COM:
							switch(choose_area(game->gk)){
								case 0:
									area(game->gk);
									print_area(game->gk,game->background_stop);
									break;
								case 1:
									break;
								case -1:
									key = ESC;
									break;
							}
							break;
						case REACTION_USER:
						case REACTION_COM:
							key = ler_tecla();
							if(key == ENTER)
								time=SEC(4);
							break;
						default:
							key = ler_tecla();
							break;
					}
										
				}
	
				 if (msg.NOTIFY_ARG & IRQ_TIMER_HOOK){
				
					switch(game->penalty){	
						case USER:
							set_ball_pos(game->user->ball,game->background_run);
							set_ball_pos(game->user->ball,game->background_stop);
							set_player_stop(game->user, game->background_stop);
							random_area(game->gk);
							update_goalkeeper(game->gk,game->background_shoot,game->background_reaction);
							game->penalty = WAIT_SHOOT_USER;
							break;
						case RUN_USER:
							if(move_player(game->user,game->background_run) == 0){
								set_player_shoot(game->user, game->background_shoot);
								game->penalty = SHOOT_USER;
							}
							break;
						case SHOOT_USER:
							if(move_ball(game->user->ball,game->background_shoot) == 0){
								draw_ball_buffer(game->user->ball,game->background_reaction);
								game->penalty=REACTION_USER;
								goal = goal_or_fail(game->user->ball);
							}
							break;
						case REACTION_USER:
							if(time >= SEC(1) && time <= SEC(3))
								reaction_player(game->user,game->background_reaction,goal);
							else if(time > SEC(3)){
								game->penalty=WAITING;
								time=0;
							}
							time++;
							break;
						case GAMEOVER_USER:
						case GAMEOVER_COM:
							if(time % 60 == 0)
								draw_gamewall(game,2);
							else if(time > SEC(5))
								key = ESC;
							time++;
							break;
						case COM:
							set_ball_pos(game->com->ball,game->background_run);
							set_ball_pos(game->com->ball,game->background_stop);
							set_player_stop(game->com, game->background_stop);
							game->penalty = WAIT_SHOOT_COM;
							break;	
						case WAIT_SHOOT_COM:
							if(!time)
								update_ball(game->com->ball,random_goal_x(),random_goal_y());
							else if(time==SEC(2)){
								whistle_referee(game);
								print_area(game->gk,game->background_stop);
							}
							else if(time==SEC(5)){
								update_goalkeeper(game->gk,game->background_shoot,game->background_reaction);
								game->penalty = RUN_COM;
								time=0;
							}
							time++;
							break;
						case RUN_COM:
							if(move_player(game->com,game->background_run) == 0){
								set_player_shoot(game->com,game->background_shoot);
								game->penalty = SHOOT_COM;
							}
							break;
						case SHOOT_COM:
							if(move_ball(game->com->ball,game->background_shoot) == 0){
								draw_ball_buffer(game->com->ball,game->background_reaction);
								game->penalty=REACTION_COM;
								goal = goal_or_fail(game->com->ball);
							}
							break;
						case REACTION_COM:
							if(time >= SEC(1) && time <= SEC(3))
								reaction_player(game->com,game->background_reaction,goal);
							else if(time > SEC(3)){
								game->penalty=WAITING;
								time=0;
							}
							time++;
							break;
						case WAITING: 								
							if(time==SEC(1)){
								time=0;
								update_game(game);
							}
							else 
								time++;
							break;			
					} 
				}
					
				 if (msg.NOTIFY_ARG & IRQ_MOUSE_HOOK) {				
					if(mouse_handler(game->mse->packet) == 0){

					switch(game->penalty){	
						case WAIT_SHOOT_USER: 
							mouse_update(game->mse);
							if(gesture_machine(game->mse->packet,game->slope) == 0){
								whistle_referee(game);
								game->penalty = RUN_USER;
								update_slope(game->slope);
								update_ball(game->user->ball,(int) game->slope[0],(int) game->slope[1]);
							}
							break;
						} 
					}
				}		
				break;
			default:
				break;
			}}	
}

delete_game(game);
return 0;
}

