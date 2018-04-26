
#include <rc_usefulincludes.h> 
#include <roboticscape.h>
#include <stdio.h>
void on_pause_pressed();
void on_pause_released();


int get_ultrasonic_cm();

int init_success;
int byte_write_success;
int ultrasonic_cm;
float motorstate1;
float motorstate2;

int main(){

	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}	
	
	printf("\nbingo!!\n");
	init_success = rc_i2c_init(1, 0x70);
	if (init_success == 0) {
		printf("\ni2c successfully initialized\n");
	}
	else {
		printf("\ni2c not initialized\n");
	}
	

	rc_enable_motors();

	
	rc_set_pause_pressed_func(&on_pause_pressed);
	rc_set_pause_released_func(&on_pause_released);

	
	rc_set_state(RUNNING); 

	while(rc_get_state()!=EXITING) {

		if(rc_get_state()==RUNNING){

			rc_set_led(GREEN, ON);
			rc_set_led(RED, OFF);

			ultrasonic_cm = get_ultrasonic_cm();			
			printf("\n%i\n", ultrasonic_cm);
			
			motorstate1 = 1.5;
			motorstate2 = -1.5;

			if (ultrasonic_cm < 30 &&  ultrasonic_cm > 5) {
				motorstate2 = 0;
			}
			

			rc_set_motor(4, motorstate1);
			rc_set_motor(1, motorstate2);
		



		}

		else if(rc_get_state()==PAUSED){
			rc_set_led(GREEN, OFF);
			rc_set_led(RED, ON);
		}

		usleep(100000);
	}
	
	rc_cleanup(); 
	return 0;
}



int get_ultrasonic_cm() {

	uint8_t i2c_buffer1[1];
	uint8_t i2c_buffer2[1];

	rc_i2c_write_byte(1, 0x00, 0x51);
	usleep(75000);
	rc_i2c_read_byte(1, 0x02, i2c_buffer1);
	rc_i2c_read_byte(1, 0x03, i2c_buffer2);

	int ultrasonic_cm = (i2c_buffer1[0] << 8) | i2c_buffer2[0];
	return ultrasonic_cm;
}










/*******************************************************************************
* void on_pause_released() 
*	
* Make the Pause button toggle between paused and running states.
*******************************************************************************/
void on_pause_released(){
	// toggle betewen paused and running modes
	if(rc_get_state()==RUNNING)		rc_set_state(PAUSED);
	else if(rc_get_state()==PAUSED)	rc_set_state(RUNNING);
	return;
}

/*******************************************************************************
* void on_pause_pressed() 
*
* If the user holds the pause button for 2 seconds, set state to exiting which 
* triggers the rest of the program to exit cleanly.
*******************************************************************************/
void on_pause_pressed(){
	int i=0;
	const int samples = 100;	// check for release 100 times in this period
	const int us_wait = 2000000; // 2 seconds
	
	// now keep checking to see if the button is still held down
	for(i=0;i<samples;i++){
		rc_usleep(us_wait/samples);
		if(rc_get_pause_button() == RELEASED) return;
	}
	printf("long press detected, shutting down\n");
	rc_set_state(EXITING);
	return;
}
