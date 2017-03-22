#include "About.h"

About* create_about(){
	About *about;
	about = malloc(sizeof(About));
	if(about==NULL)
		return NULL;

	about->wallpapper = loadBitmap("/home/lcom/proj/src/images/about_wall.bmp");
	about->background = calloc(HRES*VRES*2,sizeof(short));
	about->mse = create_mouse("about");
	about->option = 1;
	about->selected = 0;
	
	about->buttons = (Bitmap **) malloc(4*sizeof(Bitmap));
	about->buttons[0] = loadBitmap("/home/lcom/proj/src/images/shoot.bmp");
	about->buttons[1] = loadBitmap("/home/lcom/proj/src/images/save.bmp");
	about->buttons[2] = loadBitmap("/home/lcom/proj/src/images/credits.bmp");
	about->buttons[3] = loadBitmap("/home/lcom/proj/src/images/back_about.bmp");	
	
	about->info = (Bitmap **) malloc(3*sizeof(Bitmap));
	about->info[0] = loadBitmap("/home/lcom/proj/src/images/shoot_about.bmp");
	about->info[1] = loadBitmap("/home/lcom/proj/src/images/save_area_about.bmp");
	about->info[2] = loadBitmap("/home/lcom/proj/src/images/credits_about.bmp");
	return about;
}

void delete_about(About *about){
	int i;
	for(i=3;i>=0;i--)
		deleteBitmap(about->buttons[i]); 
	free(about->buttons);
	for(i=2;i>=0;i--)
		deleteBitmap(about->info[i]); 
	free(about->info);
	delete_mouse(about->mse);
	deleteBitmap(about->wallpapper);
	free(about->background);
	free(about);
	about = NULL;
}

void draw_about(About *about,int button){
	switch(button){
		case 0:
			draw_bitmap(about->wallpapper,0,0,0,about->background);
			break;
		case 1:
			draw_bitmap(about->buttons[0],0,0,2,about->background);
			draw_bitmap(about->info[0],0,220,2,about->background);
			break;
		case 2:
			draw_bitmap(about->buttons[1],744,0,2,about->background);
			draw_bitmap(about->info[1],0,145,2,about->background);
			break;
		case 3: 
			draw_bitmap(about->wallpapper,0,0,3,about->background);
			draw_bitmap(about->buttons[2],0,618,2,about->background);
			draw_bitmap(about->info[2],0,194,2,about->background);
			break;
		case 4:
			draw_bitmap(about->wallpapper,0,0,3,about->background);
			draw_bitmap(about->buttons[3],754,618,2,about->background);
			break;
	}
}

void about_mouse_option(About *about,int *selected){	
		//SHOOT
		if(ismouse_aboveme(24,33,about->mse)){
			if(!about->above){
				*selected=1;
				draw_about(about,*selected);
				about->above=1; 
			}
		}
		//SAVE
		else if(ismouse_aboveme(771,39,about->mse)){
			if(!about->above){
				*selected=2;
				draw_about(about,*selected);
				about->above=1; 
			}
		}
		//CREDITS
		else if(ismouse_aboveme(22,637,about->mse)){
			if(!about->above){
				*selected=3;
				draw_about(about,*selected);
				about->above=1; 
			}
		}
		//BACK
		else if(ismouse_aboveme(771,637,about->mse)){
			if(!about->above){
				*selected=4;
				draw_about(about,*selected);
				about->above=1; 
			}
		}
		//OUT
		else {
			if(about->above) {
				about->above=0;
				*selected=0;
				draw_about(about,*selected);
				}
		}

if(about->mse->LB){
	if(*selected==4)
		about->option = 0;
}
}

void about_keyboard_option(About *about,int *selected){
switch(*selected){
	case 0: 
		(*selected)++;
		draw_about(about,*selected);
		break;
	case 1: 
		(*selected)++;
		draw_about(about,*selected);
		break;
	case 2: 
		(*selected)++;
		draw_about(about,*selected);
		break;
	case 3:
		(*selected)++;
		draw_about(about,*selected);
		break;
	case 4: 
		*selected=0;
		draw_about(about,*selected);
		break;
}
}

int start_about(){

About *about;
about = create_about();
if(about == NULL)
	return 1;
draw_about(about,about->selected);
draw_mouse(about->mse,about->background);

int ipc_status;
int r;
unsigned long key=0;
message msg;

while (about->option) {
		
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
						about_keyboard_option(about,&about->selected);
					if(key == ENTER) 
						about->option = 0;
				}
				if (msg.NOTIFY_ARG & IRQ_TIMER_HOOK){}
	
				if (msg.NOTIFY_ARG & IRQ_MOUSE_HOOK) {
					if(mouse_handler(about->mse->packet) == 0){
						mouse_update(about->mse);
						about_mouse_option(about,&about->selected);
						draw_mouse(about->mse,about->background);
					} 
				}
				break;
			default:
				break;
			}
		}
}
	delete_about(about);
	return 0;
}
