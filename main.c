
#include <rc_usefulincludes.h> 
#include <roboticscape.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
void on_pause_pressed();
void on_pause_released();


int get_ultrasonic1_cm();
int get_ultrasonic2_cm();

int init_success;
int byte_write_success;
int ultrasonic1_cm;
int ultrasonic2_cm;
float motorstate1;
float motorstate2;
char letter;


static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
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

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
	  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
	  char ch;
	    initTermios(echo);
	      ch = getchar();
	        resetTermios();
		  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
	  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
	  return getch_(1);
}

int main(){

	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}	
	
	printf("\nRunning main program\n");
	
	rc_i2c_init(1, 0x71);
	rc_i2c_init(1, 0x73);
		
	rc_enable_motors();
	rc_enable_servo_power_rail();
	
	rc_set_pause_pressed_func(&on_pause_pressed);
	rc_set_pause_released_func(&on_pause_released);	
	rc_set_state(RUNNING); 

	while(rc_get_state()!=EXITING) {

		if(rc_get_state()==RUNNING){

			rc_set_led(GREEN, ON);
			rc_set_led(RED, OFF);
			
  			letter  = getch();
  			if (letter == 'w') {
				rc_set_motor(1, -1.5);
				rc_set_motor(2, 1.5);
			}
			else if (letter == 's') {
				rc_set_motor(1, 1.5);
				rc_set_motor(2, -1.5);
			}

			if (letter == 'a') {
				rc_send_servo_pulse_us(1, 0.5);
			}
			else if (letter == 'd') {
				rc_send_servo_pulse_us(1, -0.5);
			}



			/*

			int encoderPos = rc_get_encoder_pos(1);
			printf("\nEncoderPos: %i \n", encoderPos);
			
			ultrasonic1_cm = get_ultrasonic1_cm();
			ultrasonic2_cm = get_ultrasonic2_cm();
			printf("\nSkynjari 1: %i, Skynjari 2: %i", ultrasonic1_cm, ultrasonic2_cm);

			*/	
			
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
