#include "Menu.h"

int subscribe_all(){
if (kbd_subscribe() < 0 || timer_subscribe_int() < 0 || mouse_subscribe() < 0) return 1;
return 0;
}

int unsubscribe_all(){

if (kbd_unsubscribe() != 0) return 1;
if (timer_unsubscribe_int() != 0) return 1;
if (mouse_unsubscribe() != 0) return 1;
return 0;
}

int time_mode(){
int hour = rtc_get_hour();

if(DAY(hour))
	return DAY_MODE;
else if(SUNSET(hour))
	return SUNSET_MODE;
else if(NIGHT(hour))
	return NIGHT_MODE;
else
	return 0;

}

MenuInfo* create_menu(){
int mode,i;
MenuInfo *menu_start;
menu_start = malloc(sizeof(MenuInfo));
if(menu_start == NULL)
	return NULL;

mode = time_mode();

switch(mode){
	case DAY_MODE:
		menu_start->wallpapper = loadBitmap("/home/lcom/proj/src/images/day_final.bmp");
		break;
	case SUNSET_MODE:
		menu_start->wallpapper = loadBitmap("/home/lcom/proj/src/images/sunset_final.bmp");
		break;
	case NIGHT_MODE:
		menu_start->wallpapper = loadBitmap("/home/lcom/proj/src/images/night_final.bmp");
		break;
	default:
		free(menu_start);
		return NULL;
}
if(menu_start->wallpapper == NULL){
	free(menu_start);
	return NULL; 
}

menu_start->mse = create_mouse("menu");

if(menu_start->mse == NULL){
	deleteBitmap(menu_start->wallpapper);
	free(menu_start);
	return NULL;
} 

menu_start->option = MENU_START;
menu_start->game = MENU_NORMAL;
menu_start->selected = 0;
menu_start->above = 0;

menu_start->background = calloc(HRES*VRES*2,sizeof(short));
if(menu_start->background == NULL){
	delete_mouse(menu_start->mse);
	deleteBitmap(menu_start->wallpapper);
	free(menu_start);
	return NULL;
}

menu_start->buttons = (Bitmap **) malloc(7*sizeof(Bitmap));
if(menu_start->buttons == NULL){
	free(menu_start->background);
	delete_mouse(menu_start->mse);
	deleteBitmap(menu_start->wallpapper);
	free(menu_start);
	return NULL;
}
menu_start->buttons[0] = loadBitmap("/home/lcom/proj/src/images/play.bmp");
menu_start->buttons[1] = loadBitmap("/home/lcom/proj/src/images/about.bmp");
menu_start->buttons[2] = loadBitmap("/home/lcom/proj/src/images/exit.bmp");
menu_start->buttons[3] = loadBitmap("/home/lcom/proj/src/images/singleplayer.bmp");
menu_start->buttons[4] = loadBitmap("/home/lcom/proj/src/images/multiplayer.bmp");
menu_start->buttons[5] = loadBitmap("/home/lcom/proj/src/images/back.bmp");
menu_start->buttons[6] = loadBitmap("/home/lcom/proj/src/images/menu_game.bmp");
return menu_start;
}

void delete_menu(MenuInfo *menu) {
int i;
for(i=6;i>=0;i--)
	deleteBitmap(menu->buttons[i]); 
free(menu->buttons);
deleteBitmap(menu->wallpapper);
delete_mouse(menu->mse);
free(menu->background);
free(menu);
menu = NULL;
}

void draw_menu(MenuInfo *menu){
switch(menu->game){
	case MENU_NORMAL:
		draw_bitmap(menu->wallpapper,0,0,0,menu->background);
		break;
	case MENU_GAME:
		draw_bitmap(menu->buttons[6],129,460,2,menu->background);
		break;
}
menu_mouse_option(menu,&menu->selected);
}

void selected_state(MenuInfo *menu,int *selected){
switch(menu->game){
	case MENU_NORMAL:
		switch(*selected){	
			case 0:
				menu->option = MENU_START;
				break;
			case 1: 
				menu->option = MENU_PLAY;
				break;
			case 2: 
				menu->option = MENU_ABOUT;
				break;
			case 3: 
				menu->option = MENU_EXIT;
				break;
		}
		break;
	case MENU_GAME:
		switch(*selected){
			case 0:
				menu->option = MENU_START;
				break;
			case 1:
				menu->option = PLAY_SINGLE;
				break;
			case 2:
				menu->option = PLAY_MULTI;
				break;
			case 3:
				menu->option = PLAY_BACK;
				break;
	}
}
}

void menu_mouse_option(MenuInfo *menu,int *selected){
switch(menu->game){
	case MENU_NORMAL:		
		//PLAY
		if(ismouse_aboveme(173,469,menu->mse)){
			if(!menu->above){
				draw_bitmap(menu->buttons[0],129,460,2,menu->background);
				*selected=1;
				menu->above=1; 
			}
		}
		//ABOUT
		else if(ismouse_aboveme(145,572,menu->mse)){
			if(!menu->above){
				draw_bitmap(menu->buttons[1],129,460,2,menu->background);
				*selected=2;
				menu->above=1; 
			}
		}
		//EXIT
		else if(ismouse_aboveme(179,677,menu->mse)){
			if(!menu->above){
				draw_bitmap(menu->buttons[2],129,460,2,menu->background);
				*selected=3;
				menu->above=1; 
			}
		}
		//OUT
		else {
			if(menu->above) {
				menu->above=0;
				*selected=0;
				draw_menu(menu);
				}
		}
		break;
	case MENU_GAME:		
		//SINGLEPLAYER
		if(ismouse_aboveme(173,469,menu->mse)){
			if(!menu->above){
				draw_bitmap(menu->buttons[3],129,460,2,menu->background);
				*selected=1;
				menu->above=1; 
			}
		}
		//MULTIPLAYER
		else if(ismouse_aboveme(145,572,menu->mse)){
			if(!menu->above){
				draw_bitmap(menu->buttons[4],129,460,2,menu->background);
				*selected=2;
				menu->above=1; 
			}
		}
		//BACK
		else if(ismouse_aboveme(179,677,menu->mse)){
			if(!menu->above){
				draw_bitmap(menu->buttons[5],129,460,2,menu->background);
				*selected=3;
				menu->above=1; 
			}
		}
		//OUT
		else {
			if(menu->above) {
				menu->above=0;
				*selected=0;
				draw_menu(menu);
				}
		}
		break;
}

if(menu->mse->LB)
	selected_state(menu,selected);
}

void menu_keyboard_option(MenuInfo *menu,int *selected){
switch(menu->game){
	case MENU_NORMAL:
		switch(*selected){
			case 0: 
				draw_bitmap(menu->buttons[0],129,460,2,menu->background);
				(*selected)++;
				break;
			case 1: 
				draw_bitmap(menu->buttons[1],129,460,2,menu->background);
				(*selected)++;
				break;
			case 2: 
				draw_bitmap(menu->buttons[2],129,460,2,menu->background);
				(*selected)++;
				break;
			case 3: 
				*selected=0;
				draw_menu(menu);
				break;
		}
		break;
	case MENU_GAME:
		switch(*selected){
			case 0: 
				draw_bitmap(menu->buttons[3],129,460,2,menu->background);
				(*selected)++;
				break;
			case 1: 
				draw_bitmap(menu->buttons[4],129,460,2,menu->background);
				(*selected)++;
				break;
			case 2: 
				draw_bitmap(menu->buttons[5],129,460,2,menu->background);
				(*selected)++;
				break;
			case 3: 
				*selected=0;				
				draw_menu(menu);				
				break;
		}
		break;
	}
}

int start_menu(int player){
MenuInfo *menu;
menu = create_menu();
if(menu == NULL)
	return 1;
draw_menu(menu);
draw_mouse(menu->mse,menu->background);

int ipc_status;
int r;
int on=1;
unsigned long key=0;
message msg;
int start=0;

while (on) {
		
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				 if (msg.NOTIFY_ARG & IRQ_KBD_HOOK){
					key = ler_tecla();

				if(key==TAB)
					menu_keyboard_option(menu,&menu->selected);

				else if(key==ENTER)
					selected_state(menu,&menu->selected);
				}
	
				if (msg.NOTIFY_ARG & IRQ_TIMER_HOOK){}
					
				 if (msg.NOTIFY_ARG & IRQ_MOUSE_HOOK) {				
					if(mouse_handler(menu->mse->packet) == 0){
						mouse_update(menu->mse);
						menu_mouse_option(menu,&menu->selected);
						draw_mouse(menu->mse,menu->background);
					} 
				}				
				break;
			default:
				break;
			}}
		
	switch(menu->option){
		case MENU_START:
			if(start){
				menu->selected = 0;
				menu->above=0;
				menu->mse->LB=0;
				draw_menu(menu);
				start=0;
				}				
			break;
		case MENU_PLAY:
			start=1;
			menu->game = MENU_GAME;
			menu->option = MENU_START;
			break;
		case MENU_ABOUT:
			start_about();
			start=1;
			menu->option = MENU_START;
			break;
		case PLAY_SINGLE:
			start_game();
			start=1;
			menu->option = MENU_START;
			menu->game = MENU_NORMAL;
			break;
		case PLAY_MULTI:
			start_game_multi(player);
			start=1;
			menu->option = MENU_START;
			menu->game = MENU_NORMAL;
			break;
		case PLAY_BACK:
			start=1;
			menu->option = MENU_START;
			menu->game = MENU_NORMAL;
			break;
		case MENU_EXIT:
			on=0;
			break; 
	}				
}

delete_menu(menu);
return 0;
}
