#include "game_multi.h"

Game_Multi* create_game_multi(){
Game_Multi *game;
int i;

game = malloc(sizeof(Game));
if(game == NULL) 
	return NULL;

game->wallpapper = loadBitmap("/home/lcom/proj/src/images/game_wall_multi.bmp");

game->background = calloc(HRES*VRES*2,sizeof(short));
game->background_stop = calloc(HRES*VRES*2,sizeof(short));
game->background_run = calloc(HRES*VRES*2,sizeof(short));

game->background_shoot = (short **) calloc(3,sizeof(short*));
for(i=0;i<3;i++)
	game->background_shoot[i] = (short *) calloc(HRES*VRES*2,sizeof(short));
game->background_reaction = calloc(HRES*VRES*2,sizeof(short));

game->score = (Bitmap **) malloc(2*sizeof(Bitmap*));

game->score[0] = loadBitmap("/home/lcom/proj/src/images/goal.bmp");
game->score[1] = loadBitmap("/home/lcom/proj/src/images/fail.bmp");

game->wall_wait = (Bitmap **) malloc(6*sizeof(Bitmap*));
game->wall_wait[0] = loadBitmap("/home/lcom/proj/src/images/game_multi_wait.bmp");
game->wall_wait[1] = loadBitmap("/home/lcom/proj/src/images/j1.bmp");
game->wall_wait[2] = loadBitmap("/home/lcom/proj/src/images/j2.bmp");
game->wall_wait[3] = loadBitmap("/home/lcom/proj/src/images/ready_multi.bmp");
game->wall_wait[4] = loadBitmap("/home/lcom/proj/src/images/youare_p1.bmp");
game->wall_wait[5] = loadBitmap("/home/lcom/proj/src/images/youare_p2.bmp");

game->numbers = loadBitmap("/home/lcom/proj/src/images/numbers.bmp");

game->gameover = (Bitmap **) malloc(6*sizeof(Bitmap*));
game->gameover[0] = loadBitmap("/home/lcom/proj/src/images/p1_small.bmp");
game->gameover[1] = loadBitmap("/home/lcom/proj/src/images/p1_big.bmp");
game->gameover[2] = loadBitmap("/home/lcom/proj/src/images/p2_small.bmp");
game->gameover[3] = loadBitmap("/home/lcom/proj/src/images/p2_big.bmp");
game->gameover[4] = loadBitmap("/home/lcom/proj/src/images/p1_out.bmp");
game->gameover[5] = loadBitmap("/home/lcom/proj/src/images/p2_out.bmp");

game->referee = (Bitmap **) malloc(2*sizeof(Bitmap*));

game->referee[0] = loadBitmap("/home/lcom/proj/src/images/ref0.bmp");
game->referee[1] = loadBitmap("/home/lcom/proj/src/images/ref1.bmp");

game->mse = create_mouse("game");
game->slope = (short *) malloc(2*sizeof(short));
game->slope[0] = 0;
game->slope[1] = 0;
game->p1 = create_player("user");
game->p2 = create_player("com");
game->gk = create_goalkeeper();
game->penalty = WAIT_CONNECTION;
game->mode = NORMAL_M;
game->shooting = WAIT_CON;
game->key = 0;
return game;
}

void delete_game_multi(Game_Multi *game){
int i;
delete_mouse(game->mse);
free(game->slope);
delete_goalkeeper(game->gk);
delete_player(game->p1);
delete_player(game->p2);
for(i=1;i>=0;i--)
	deleteBitmap(game->score[i]);
free(game->score);
for(i=5;i>=0;i--)
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
for(i=5;i>=0;i--)
	deleteBitmap(game->wall_wait[i]);
free(game->wall_wait);
free(game);
game = NULL;
}

void whistle_referee_multi(Game_Multi *game){
	draw_bitmap(game->referee[1],0,302,2,game->background_stop);
}

void draw_gamewall_multi(Game_Multi *game,int state,int player){
static int i=0;
if(!state)
	draw_bitmap(game->wallpapper,0,0,0,game->background);
else if(state==3){
	draw_bitmap(game->wall_wait[0],0,0,0,game->background);
	if(player == 1)
		draw_bitmap(game->wall_wait[player],357,392,2,game->background);
	else
		draw_bitmap(game->wall_wait[player],600,392,2,game->background);		
	return;
}
else
	draw_buffer(game->background);

//GOALS
drawBitmap_number(game->numbers,"user",game->p1->goals,game->background);
drawBitmap_number(game->numbers,"com",game->p2->goals,game->background);
if(state==2){
	switch(game->penalty){
		case GAMEOVER_P1:
			if(i%2==0)
				draw_bitmap(game->gameover[1],65,299,1,game->background);
			else
				draw_bitmap(game->gameover[0],283,346,1,game->background);
			i++;
			return;
		case GAMEOVER_P2:
			if(i%2==0)
				draw_bitmap(game->gameover[3],65,299,1,game->background);
			else
				draw_bitmap(game->gameover[2],283,346,1,game->background);
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

void reset_gamewall_multi(Game_Multi *game){
	draw_gamewall_multi(game,0,0);
	game->p1->attempts = 0;
	game->p2->attempts = 0;
}

void update_game_multi(Game_Multi *game){
static int i=0;

if(!game->p1->attempts)
	i=0;

//NEXT PENALTY
if(i%2 == 0){
	if(goal_or_fail(game->p1->ball))
		draw_bitmap(game->score[1],320+28*game->p1->attempts,66,2,game->background);
	else{
		draw_bitmap(game->score[0],320+28*game->p1->attempts,66,2,game->background);
		game->p1->goals++;
	}
	game->p1->attempts++;
	game->penalty = P2;
	reset_gk(game->gk);
}
else{
	if(goal_or_fail(game->p2->ball))
		draw_bitmap(game->score[1],570+28*game->p2->attempts,66,2,game->background);
	else{
		draw_bitmap(game->score[0],570+28*game->p2->attempts,66,2,game->background);
		game->p2->goals++;
	}
	game->p2->attempts++;
	game->penalty = P1;
	reset_gk(game->gk);
}
i++;
draw_gamewall_multi(game,1,0);
//LOGIC
switch(game->mode){
	case NORMAL_M: 
		if( (game->p1->goals > ( 5 - game->p2->attempts + game->p2->goals)) ){
			game->penalty = GAMEOVER_P1;
			i=0;
		}
		else if( (game->p2->goals > ( 5 - game->p1->attempts + game->p1->goals)) ){
			game->penalty = GAMEOVER_P2;
			i=0;
		}
		else if( (game->p1->goals == game->p2->goals) && (game->p1->attempts == game->p2->attempts) && (game->p1->attempts == 5)){
			game->mode = SUDDEN_DEATH_M;
			reset_gamewall_multi(game);
		}
		break;
	
	case SUDDEN_DEATH_M:
		if( (game->p1->goals > game->p2->goals) && (game->p1->attempts == game->p2->attempts) ){
			game->penalty = GAMEOVER_P1;
			i=0;
		}
		else if( (game->p1->goals < game->p2->goals) && (game->p1->attempts == game->p2->attempts) ){
			game->penalty = GAMEOVER_P2;
			i=0;	
		}
		else if(game->p2->attempts == 5)
			reset_gamewall_multi(game);
		break;
}

}

int start_game_multi(int player){

Game_Multi *game;
game = create_game_multi();

if(game == NULL) 
	return 1;

if( serial_subscribe() == -1 )
	return 1;

int ipc_status;
int r;
unsigned long key=0;
message msg;
int time=0,i=0,goal=0,ii=0;
Ball *ball_player;
unsigned char key_send,key_receive;
if(player==1){
	ball_player = game->p1->ball;
	key_receive = KEY_MATCH + (unsigned char) 2;
}
else{
	ball_player = game->p2->ball;
	key_receive = KEY_MATCH + (unsigned char) 1;
}

key_send = KEY_MATCH + (unsigned char) player;
while (key != ESC) {		
	if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
		printf("driver_receive failed with: %d", r);
		continue;
	}
	if (is_ipc_notify(ipc_status)) {
		switch (_ENDPOINT_P(msg.m_source)) {
		case HARDWARE:
			 if (msg.NOTIFY_ARG & IRQ_KBD_HOOK){
				switch(game->shooting){
					case WAIT_CON:
						key = ler_tecla();
						break;
					case PLAYING:
						switch(game->penalty){
							case WAIT_SHOOT_P1:
								if(player==2){
									switch(choose_area(game->gk)){
										case 0:
											area(game->gk);
											serial_send_area(game->gk->save_area);
											print_area(game->gk,game->background_stop);
											break;
										default:
											break;
										case -1:
											key = ESC;
											break;
									}
								}
								else
									key = ler_tecla();
								break;
							case WAIT_SHOOT_P2:
								if(player==1){
									switch(choose_area(game->gk)){
										case 0:
											area(game->gk);
											serial_send_area(game->gk->save_area);
											print_area(game->gk,game->background_stop);
											break;
										case 1:
											break;
										case -1:
											key = ESC;
											break;
									}
								}
								else
									key = ler_tecla();
								break;
							default:
								key = ler_tecla();
								break;
						}
						if(key == ESC)
							serial_send_char((unsigned char) ESC);
						break;						
				}	
			}

			 if (msg.NOTIFY_ARG & IRQ_TIMER_HOOK){
		
				switch(game->penalty){	
					case WAIT_CONNECTION:
						if(!i){
							serial_send_char(key_send);
							draw_gamewall_multi(game,3,player);
							i=1;
						}
						if(!(time%5))
							draw_ball_waiting(ball_player,game->background);
						time++;	
						break;
					case READY:
						if(!time){
							draw_bitmap(game->wall_wait[3],345,298,3,game->background);
							if(player==1){
								draw_bitmap(game->wall_wait[2],600,392,3,game->background);
								draw_bitmap(game->wall_wait[4],352,668,2,game->background);
							}
							else{
								draw_bitmap(game->wall_wait[1],357,392,3,game->background);
								draw_bitmap(game->wall_wait[5],352,668,2,game->background);
							}
						}
						else if(time >= SEC(4)){
							i=0;
							game->penalty = P1;
							draw_gamewall_multi(game,0,player);
							time=0;
						}
						time++;
						break;
					case DISCONNECTED:
						if(!time){
							if(player==1)
								draw_bitmap(game->gameover[5],44,298,2,game->background);
							else
								draw_bitmap(game->gameover[4],44,298,2,game->background);
						}
						else if(time >=SEC(4))
							key=ESC;
						time++;
						break;							
					case P1:
						set_ball_pos(game->p1->ball,game->background_run);
						set_ball_pos(game->p1->ball,game->background_stop);
						set_player_stop(game->p1, game->background_stop);
						time=0;
						game->penalty = WAIT_SHOOT_P1;
						break;
					case RUN_P1:
						if(time >= SEC(1)){
							if(move_player(game->p1,game->background_run) == 0){
								set_player_shoot(game->p1, game->background_shoot);
								game->penalty = SHOOT_P1;
								time=0;
							}
						}
						time++;
						break;
					case SHOOT_P1:
						if(move_ball(game->p1->ball,game->background_shoot) == 0){
							draw_ball_buffer(game->p1->ball,game->background_reaction);
							game->penalty=REACTION_P1;
							goal = goal_or_fail(game->p1->ball);
						}
						break;
					case REACTION_P1:
						if(time >= SEC(1) && time <= SEC(3))
							reaction_player(game->p1,game->background_reaction,goal);
						else if(time > SEC(3)){
							game->penalty=WAITING;
							time=0;
						}
						time++;
						break;
					case GAMEOVER_P1:
					case GAMEOVER_P2:
						if(time > SEC(5))
							key = ESC;
						else if(time % 60 == 0)
							draw_gamewall_multi(game,2,player);
						time++;
						break;
					case P2:
						set_ball_pos(game->p2->ball,game->background_run);
						set_ball_pos(game->p2->ball,game->background_stop);
						set_player_stop(game->p2, game->background_stop);
						time=0;
						game->penalty = WAIT_SHOOT_P2;
						break;
					case RUN_P2:
						if(time >= SEC(1)){
							if(move_player(game->p2,game->background_run) == 0){
								set_player_shoot(game->p2, game->background_shoot);
								game->penalty = SHOOT_P2;
								time=0;
							}
						}
						time++;
						break;
					case SHOOT_P2:
						if(move_ball(game->p2->ball,game->background_shoot) == 0){
							draw_ball_buffer(game->p2->ball,game->background_reaction);
							game->penalty=REACTION_P2;
							goal = goal_or_fail(game->p2->ball);
						}
						break;
					case REACTION_P2:
						if(time >= SEC(1) && time <= SEC(3))
							reaction_player(game->p2,game->background_reaction,goal);
						else if(time > SEC(3)){
							game->penalty=WAITING;
							time=0;
						}
						time++;
						break;
					case WAITING: 								
						if(time==SEC(1)){
							time=0;
							update_game_multi(game);
						}
						else 
							time++;
						break;			
				} 
			}
			
			 if (msg.NOTIFY_ARG & IRQ_MOUSE_HOOK) {				
				if(mouse_handler(game->mse->packet) == 0){
					mouse_update(game->mse);
					switch(game->penalty){	
						case WAIT_SHOOT_P1:							
							if(gesture_machine(game->mse->packet,game->slope) == 0){
								update_slope(game->slope);
								switch(player){
									case 1:
										update_ball(game->p1->ball,(int) game->slope[0],(int) game->slope[1]);
											game->penalty = RUN_P1;
											whistle_referee_multi(game);
											serial_send_shoot(game->slope);
											update_goalkeeper(game->gk,game->background_shoot,game->background_reaction);
											time=0;
										break;
									case 2:
										break;
								}
							}
							break;
						case WAIT_SHOOT_P2:
							if(gesture_machine(game->mse->packet,game->slope) == 0){
								update_slope(game->slope);
								switch(player){
									case 1:
										break;
									case 2:
										update_ball(game->p2->ball,(int) game->slope[0],(int) game->slope[1]);
										game->penalty = RUN_P2;
										whistle_referee_multi(game);
										serial_send_shoot(game->slope);
										update_goalkeeper(game->gk,game->background_shoot,game->background_reaction);
										time=0;
										break;
								}
							}
							break;
						default:
							break;
						}
				}
			}
		
			if (msg.NOTIFY_ARG & IRQ_SERIAL_HOOK) {
				serial_handle(game);
				switch(game->shooting){	
					case WAIT_CON:
						if(game->key == key_receive){
							serial_send_char(key_send);						
							game->penalty = READY;
							game->shooting = PLAYING;
							time=0;
							ii=0;
						}
						break;
					case PLAYING:
						if(game->key == ESC){
							game->penalty = DISCONNECTED;
							time=0;
						}
						switch(game->penalty){
							case WAIT_SHOOT_P1:
								switch(player){
									case 1:
										serial_read_area(game,game->key);
										break;
									case 2:
									if(serial_read_shoot(game,game->p1->ball,game->key) == 0){
										whistle_referee_multi(game);
										update_goalkeeper(game->gk,game->background_shoot,game->background_reaction);			
										game->penalty = RUN_P1;
										time=0;
									}
										break;	
								}
								break;
							case WAIT_SHOOT_P2:
								switch(player){
									case 2:
										serial_read_area(game,game->key);
										break;
									case 1:
									if(serial_read_shoot(game,game->p2->ball,game->key) == 0){
										whistle_referee_multi(game);
										update_goalkeeper(game->gk,game->background_shoot,game->background_reaction);				
										game->penalty = RUN_P2;
										time=0;
									}
										break;	
								}
								break;
							default:
								break;
						}				
						break;
				}
			}
			break;
		default:
			break;
	}
	}
	game->key = 0;
}
serial_unsubscribe();
delete_game_multi(game);
return 0;
}
