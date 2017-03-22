#include <minix/sysutil.h>
#include "test4.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8042.h"
#include "i8254.h"
#include "timer.h"
#include <string.h>

int test_packet(unsigned short cnt){
	unsigned long status, tmp,info;  
	unsigned char packet[3];	
	int ipc_status;
	int irq_set = BIT(mouse_subscribe());
	int r;
	int i=0;
	int byte;
	message msg;

	if (irq_set < 0) {
		printf("Erro ao ligar o interrupt\n");
		return 1;
	}
	
	while(1){
	escrever_mouse_1();
	escrever_mouse_2(ENABLE_DATA);
	ler_mouse(&info);
	if (info == ACK) 
		break; }
	
	while(i < cnt) {
		if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("Driver_receive failed\n");
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			/* received notification */
			
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */			
					byte = mouse_handler(packet);					
					if(byte == 0) {
						print_packet(packet); 
						i++; }								
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
				}}
	while(1){
	escrever_mouse_1();
	escrever_mouse_2(DISABLE_DATA);
	escrever_mouse_1();
	escrever_mouse_2(ENABLE_STREAM_MODE);
	ler_mouse(&info);
	if (info == ACK) 
		break; }

	if(mouse_unsubscribe() != 0)
		return 1;
	//Esvaziar OUT_BUF
		sys_inb(OUT_BUF, &tmp); 		
	return 0;
}	
	
int test_async(unsigned short idle_time) {
	unsigned long status, tmp,info;  
	unsigned char packet[3];	
	int ipc_status;
	unsigned int irq_set_mouse, irq_set_timer;
	int r;
	int i=0;
	int byte;
	int segundos = idle_time,contador=0;
	message msg;
	
	irq_set_mouse = BIT(mouse_subscribe());
	irq_set_timer = BIT(timer_subscribe_int());
	
	if (irq_set_mouse < 0 || irq_set_timer < 0) {
		printf("Erro ao ligar o interrupt\n");
		return 1;
	}
			
	while(1){
	escrever_mouse_1();
	escrever_mouse_2(ENABLE_STREAM_MODE);
	escrever_mouse_1();
	escrever_mouse_2(ENABLE_DATA);	
	ler_mouse(&info);
	if (info == ACK) 
		break; }
	
	while(contador < idle_time*60 ) {
		if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("Driver_receive failed\n");
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			/* received notification */			
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_mouse) { /* subscribed interrupt */			
					contador = 0;
					segundos = idle_time;					
					byte = mouse_handler(packet);					
					if(byte == 0) 
						print_packet(packet);  }
								
				
				else if (msg.NOTIFY_ARG & irq_set_timer){
					contador++; //Incrementa contador
					if (contador % 60 == 0 && contador != 0) {
						printf("%ds\n", segundos);
						segundos--;}
				}							
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
				}}
	printf("Finish!\n");
	if (timer_unsubscribe_int() != 0)
		return 1; 

	while(1){
	escrever_mouse_1();
	escrever_mouse_2(DISABLE_DATA);
	escrever_mouse_1();
	escrever_mouse_2(ENABLE_STREAM_MODE);
	ler_mouse(&info);
	if (info == ACK) 
		break; }

	if(mouse_unsubscribe() != 0)
		return 1;
	//Esvaziar OUT_BUF
		sys_inb(OUT_BUF, &tmp); 	  
	return 0;
}
	
int test_config(void) {
    /* No 1º Byte vem : Bit 6 - Remote, Bit 5 - Enable, Bit 4 - Scaling, Bit 2 - Left, Bit 1 - Middle, Bit 0 - Right
       No 2º Byte vem : Bit 1 e 0 - Resolution
       No 3º Byte vem : Sample Rate

	Remote:   1 = Remote (polled) mode, 0 = Stream mode. 
	Enable:     1 = Data reporting enabled, 0 = disabled.  This bit only has effect in Stream mode. 
	Scaling:    1 = Scaling is 2:1, 0 = scaling is 1:1.  See commands $E6 and $E7 below. 
	Left: 1 = Left button is currently pressed, 0 = released. 
	Middle:    1 = Middle button is currently pressed, 0 = released. 
	Right: 1 = Right button is currently pressed, 0 = released. 
	Resolution:  Legal argument values are $00, $01, $02, and $03, corresponding to resolutions of 1, 2, 4, and 8 counts per mm, respectively. 
	Sample rate:  The current sample rate setting, from 10 to 200 as described under Set Sample Rate ($F3) above. */
	
	unsigned long info,status_1,tmp;
	unsigned char byte, status[3];	
	int i;
	
	if(mouse_subscribe() == -1) 
		return 1;
	
	while(1){
	escrever_mouse_1();
	escrever_mouse_2(STATUS_REQUEST);
	ler_mouse(&info);
	if (info == ACK) 
		break; } // ACK como resposta, seguido por um status de 3 bytes 
	
	for(i=0;i<3;i++){
	ler_mouse(&info);
	byte = (char) info; 
	status[i] = byte;}

	//Analisar 1º Byte
	if(bit_char(status[0],6)) 
		printf("Mode -> Remote\n");
	else
		printf("Mode -> Stream\n");

	if(bit_char(status[0],5)) 
		printf("Data Reporting -> Enabled\n");
	else
		printf("Data Reporting -> Disabled\n");
	
	if(bit_char(status[0],4)) 
		printf("Scaling -> 2:1\n");
	else
		printf("Scaling -> 1:1\n");

	if(bit_char(status[0],2)) 
		printf("Left Button -> Pressed\n");
	else
		printf("Left Button -> Released\n");

	if(bit_char(status[0],1)) 
		printf("Middle Button -> Pressed\n");
	else
		printf("Middle Button -> Released\n");

	if(bit_char(status[0],0)) 
		printf("Right Button -> Pressed\n");
	else
		printf("Right Button -> Released\n");


	//Analisar 2º Byte
	switch (status[1]) {
	case 0:
		printf("Resolution -> 1 count per mm\n");
		break;
	case 1:
		printf("Resolution -> 2 counts per mm\n");
		break;
	case 2:
		printf("Resolution -> 4 counts per mm\n");
		break;
	case 3:
		printf("Resolution -> 8 counts per mm\n");
		break; }
	
	//ANALISAR 3º Byte
	printf("Sample Rate -> %d\n",status[2]);
	
	if(mouse_unsubscribe() != 0)
		return 1;

	return 0; 
}	
	
int test_gesture(short length) {
    /*Igual à funcao test_packet() só que a condicao de saída é diferente: se o utilizador mexer o rato para cima a clicar no lado direito do rato...*/
	/*Para sair o utilizador tem de o fazer numa distância miníma especificada pela variável length*/

	/*Arranjar uma maneira de ver se nos ultimos $lenght estados o y subiu sempre e o RB estava premido*/
	//Posso fazer uma funcao que verifica, em cada IH, o valor do bit do RB e compara o valor anterior de y com o valor de y acabado de ler.
	//Se os dois testes derem positivo incrementa um contador, caso contrario da reset ao contador
	//Se o contador tiver o mesmo valor que length, sai do ciclo

	unsigned long status, tmp,info;
	unsigned char packet[3];
	int ipc_status;
	int irq_set = BIT(mouse_subscribe());
	int r,negativo=0;
	int byte;
	int estado=1;
	message msg;

	if (irq_set < 0) {
		printf("Erro ao ligar o interrupt\n");
		return 1;
	}

	while(1){
	escrever_mouse_1();
	escrever_mouse_2(ENABLE_DATA);
	ler_mouse(&info);
	if (info == ACK) 
		break; }
	
	if (length < 0) {
		length *= -1;
		negativo = 1; }

	while(estado) {
		if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("Driver_receive failed\n");
			continue;
			}
		if (is_ipc_notify(ipc_status)) {
		/* received notification */

		switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					byte = mouse_handler(packet);
					if(byte == 0) {
						print_packet(packet);
						estado = gesture_machine(packet,length,negativo);			
							}
					break;
				default:
					break; /* no other notifications expected: do nothing */
				}
					}}}
		while(1){
		escrever_mouse_1();
		escrever_mouse_2(DISABLE_DATA);
		escrever_mouse_1();
		escrever_mouse_2(ENABLE_STREAM_MODE);
		ler_mouse(&info);
		if (info == ACK) 
			break; }

		if(mouse_unsubscribe() != 0)
			return 1;
		//Esvaziar OUT_BUF
			sys_inb(OUT_BUF, &tmp);
		return 0;
}
