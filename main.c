
#include <rc_usefulincludes.h> 
#include <roboticscape.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include "/usr/include/flite/flite.h"
#include <stdbool.h>
//#include "cv.h"
//#include "highgui.h"
void on_pause_pressed(); 
void on_pause_released();

//helper functions
int get_ultrasonic1_cm();
int get_ultrasonic2_cm();
void manual_input(char letter);
void forward();
void back();
void turn_right();
void turn_left();
void hold();

// manual input functions
void initTerminos(int echo);
void resetTerminos(void);
char getch_(int echo);
char getch(void);
char getche(void);

static struct termios old, new;


int main(){
	
	int cm_left;
	int cm_right;
	//int left_wheel_count;
	//int right_wheel_count;
	char letter;
	bool start = true;
	bool first = true;
	bool first_accel = true;
	//cst_voice *v;	
	rc_imu_data_t accel_data;
	float currXaccel, currYaccel, currZaccel;
	float previousXaccel = 0, previousYaccel = 0, previousZaccel = 0;
	float accelerodata[10000];
	int counter = 0;

	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}	
	
	printf("\nRunning main program\n");
	
	rc_imu_config_t rc_get_default_imu_config();
	rc_imu_config_t config;
	rc_set_imu_config_to_defaults(&config);

       	rc_initialize_imu(&accel_data, config);	
	//flite_init();
	rc_i2c_init(1, 0x71);
	rc_i2c_init(1, 0x73);	
	rc_enable_motors();
	
	
	rc_set_pause_pressed_func(&on_pause_pressed);
	rc_set_pause_released_func(&on_pause_released);	
	rc_set_state(RUNNING); 

	while(rc_get_state()!=EXITING) {
		if(rc_get_state()==RUNNING){
			rc_set_led(GREEN, ON);
			rc_set_led(RED, OFF);
			
			//  MANUAL CONTROL -------------------	
			
			/*
  			letter  = getch();
			manual_input(letter); */
  			
			//------------------------------------  	
			
			/*
			v = register_cmu_us_kal();
			flite_file_to_speech("HELLO",v,"play");
			
			left_wheel_count = rc_get_encoder_pos(1);
			right_wheel_count = rc_get_encoder_pos(2);
			*/

	
			
			//cm_left = get_ultrasonic1_cm();
			//cm_right = get_ultrasonic2_cm();

			//printf("\nLeft wheel: %i, Right wheel: %i", left_wheel_count, right_wheel_count); 
			
			//printf("\nLeft sensor: %i, Right sensor: %i", cm_left, cm_right);
			
	
			
			if (start == true) {
				letter = getch();
				if (letter == 'w'){
					start = false;
				}	
			}
			
			
			if (first == true && start == false) {
			for (float i = 0; i < 1.5; i+= 0.1) {
		
				rc_set_motor(1, i);
				rc_set_motor(2, -i);
				usleep(100000);
						
			}
			first = false;
			}
			
			rc_read_accel_data(&accel_data);

			currXaccel = accel_data.accel[0]*10;
			currYaccel = accel_data.accel[1]*10;
			currZaccel = accel_data.accel[2]*10;

			printf("\n %f", currXaccel);			

			/*					
			
			if (currXaccel >  previousXaccel + 20 ||currXaccel < previousXaccel - 25)  {
				
				back();
				sleep(1);
				turn_right();
				forward();
			}
							
				
			printf("\n X: %f, Y: %f", previousXaccel, previousYaccel); 
	
			printf("\n X: %f, Y: %f", currXaccel, currYaccel); 
			
				
			if ((cm_left < 30 ||cm_right < 30) && (cm_left > 5 ||cm_right > 5) && (start == false)) {
				
					
				hold();
				if (cm_left < cm_right) {
					turn_right();
				}
				else if (cm_right < cm_left) {
					turn_left();
				}
				
			}
				*/
			
			forward();	
							
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

void forward() {
	rc_set_motor(1, 1.5);
	rc_set_motor(2, -1.5);
}

void back() {
	rc_set_motor(1, -1.5);
	rc_set_motor(2, 1.5);
}

void turn_right() {

	rc_set_motor(1, 0.2);
	rc_set_motor(2, 1.5);
	usleep(850000);
}

void turn_left() {

	rc_set_motor(1, -1.5);
	rc_set_motor(2, -0.2);
	usleep(850000);
}

void hold() {
	rc_set_motor(1, 0);
	rc_set_motor(2, 0);
	usleep(100000);
}


void manual_input(char letter) {


  	if (letter == 'w') {
		rc_set_motor(1, -1.5);
		rc_set_motor(2, 1.5);
	}
	else if (letter == 's') {
		rc_set_motor(1, 1.5);
		rc_set_motor(2, -1.5);
	}
	else if (letter == ' ') {
		rc_set_motor(1, 0);
		rc_set_motor(2, 0);
	}
	if (letter == 'a') {
		rc_set_motor(1, 0.2);
	}
	else if (letter == 'd') {		
		rc_set_motor(2, 0.2);
	}
}


int get_ultrasonic1_cm() {

	uint8_t i2c_buffer1[1];
	uint8_t i2c_buffer2[1];
	rc_i2c_set_device_address(1, 0x71);
	rc_i2c_write_byte(1, 0x00, 0x51);
	usleep(75000);
	rc_i2c_read_byte(1, 0x02, i2c_buffer1);
	rc_i2c_read_byte(1, 0x03, i2c_buffer2);

	int ultrasonic_cm = (i2c_buffer1[0] << 8) | i2c_buffer2[0];
	return ultrasonic_cm;
}

int get_ultrasonic2_cm() {

	uint8_t i2c_buffer1[1];
	uint8_t i2c_buffer2[1];
	rc_i2c_set_device_address(1, 0x73);
	rc_i2c_write_byte(1, 0x00, 0x51);
	usleep(75000);
	rc_i2c_read_byte(1, 0x02, i2c_buffer1);
	rc_i2c_read_byte(1, 0x03, i2c_buffer2);

	int ultrasonic_cm = (i2c_buffer1[0] << 8) | i2c_buffer2[0];
	return ultrasonic_cm;
}

void initTermios(int echo) {

	tcgetattr(0, &old); /* grab old terminal i/o settings */
	new = old; /* make new settings same as old settings */
	new.c_lflag &= ~ICANON; /* disable buffered i/o */
		if (echo) {
		      new.c_lflag |= ECHO; /* set echo mode */
		}
	       	else {
		      new.c_lflag &= ~ECHO; /* set no echo mode */
					        }
		  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

void resetTermios(void) {
	  tcsetattr(0, TCSANOW, &old);
}

char getch_(int echo) {
	  char ch;
	    initTermios(echo);
	      ch = getchar();
	        resetTermios();
		  return ch;
}

char getch(void) {
	  return getch_(0);
}


char getche(void) {
	  return getch_(1);
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
