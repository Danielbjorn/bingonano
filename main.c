#include </usr/include/python2.7/Python.h>
#include <rc_usefulincludes.h> 
#include <roboticscape.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include "flite/flite.h"
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
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
void small_turn_right();
void small_turn_left();
void slow_forward();
void slow_forward_left();
void slow_forward_right();
void slow_right();
void slow_left();
void bananaDance();
void hold();

//Threads
#define NO_OF_THREADS 3
void *Ultrasonic_thread(void *vargp);
void *Gyro_thread(void *vargp);
void *Accel_thread(void *vargp);

// Manual input functions
void initTerminos(int echo);
void resetTerminos(void);
char getch_(int echo);
char getch(void);
char getche(void);

//Global variables
int cm_left;
int cm_right;
float GyroX;
int cm_left;
int cm_right;
float GyroX;
float AccelX;
float AccelY;
rc_imu_data_t accel_data;
bool GyroDisable = false;
bool collision = false;
bool spinning = false;
long bananaHunt = 0;

// PYTHON
void *Python_thread(void *vargp);
bool pyThread = true;


static struct termios old, new;

int main(){
	
	char letter;
	bool start = true;
	bool first = true;
	int counter = 0;
	//bool first_accel = true;
	//float currXaccel;


	if(rc_initialize()){
		fprintf(stderr,"ERROR: failed to initialize rc_initialize(), are you root?\n");
		return -1;
	}	
	
	printf("\nRunning main program\n");
	
	rc_imu_config_t rc_get_default_imu_config();
	rc_imu_config_t config;
	rc_set_imu_config_to_defaults(&config);

       	rc_initialize_imu(&accel_data, config);	
	rc_i2c_init(1, 0x71);
	rc_i2c_init(1, 0x73);	
	rc_enable_motors();
	
	//Py_SetPrefix("/usr/local");
	//char* name = Py_GetExPrefix();

	//printf("Prefix: %s", name);
	
	

	//Py_SetPythonHome("/usr/lib/python2.7");
	//Py_SetProgramName("/usr/bin/python2.7");
	//char* path = Py_GetPath();
	//printf("path: %s", path);

	Py_Initialize();
		
	PySys_SetPath("/usr/local/modules");
	
	
	rc_set_pause_pressed_func(&on_pause_pressed);
	rc_set_pause_released_func(&on_pause_released);	
	rc_set_state(RUNNING); 


	pthread_t thread_id;
	pthread_create(&thread_id, NULL, Ultrasonic_thread, NULL);
	pthread_create(&thread_id, NULL, Gyro_thread, NULL);
	pthread_create(&thread_id, NULL, Accel_thread, NULL);
	pthread_create(&thread_id, NULL, Python_thread, NULL);

	//system("aplay knight.wav");
	//system("^C");
	//python_file_test();

	while(rc_get_state()!=EXITING) {
		if(rc_get_state()==RUNNING){

			rc_set_led(GREEN, ON);
			rc_set_led(RED, OFF);
			
			//  MANUAL CONTROL -------------------	
			
			/*
  			letter  = getch();
			manual_input(letter); */
  			
			//------------------------------------

		       //printf("\nAccel X: %f, Y: %f, Z: %f", accel_data.accel[0], accel_data.accel[1], accel_data.accel[2]);
		       //system("^C");
		       //printf("BananaHunt is: %li\n", bananaHunt);
			
			if(bananaHunt == 0) {
				//printf("Searching\n");
				//system("aplay knight.wav &");
			
				if (collision == true) {
					GyroDisable = true;
					back();
				
					if(cm_left < cm_right) {
						turn_right();
					} else if(cm_right < cm_left) {
						turn_left();
					}
					collision = false;
					GyroDisable = false;
				}

				if ((cm_left < 65 ||cm_right < 65) && (cm_left > 45 ||cm_right > 45))   {

					if (cm_left < cm_right) {
						small_turn_right();
					}

					else if (cm_right < cm_left) {
						small_turn_left();
					}
				}
				else if ((cm_left < 35 ||cm_right < 35) && (cm_left > 10 ||cm_right > 10)) {		
					GyroDisable = true;
					hold();
					if (cm_left < cm_right) {
						turn_right();
					}
					else if (cm_right < cm_left) {
						turn_left();
					}
					GyroDisable = false;
									
				}

				if(spinning) {
					GyroDisable = true;
					system("flite -t move");
					back();
				
					if(cm_left < cm_right) {
						turn_right();
					}
					else if(cm_right < cm_left) {
						turn_left();
					}
					spinning = false;
					GyroDisable = false;
				}
/*
			 if ((GyroX > 60 ||GyroX < -60) && counter > 15) {

				system("flite -t move");
				back();

				if(cm_left < cm_right) {
					turn_right();
				}

				else if(cm_right < cm_left) {
					turn_left();
				}
				counter = 0;
			
 			 }	
*/
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
															
				forward();
				counter++;	
							
			}
			else if(bananaHunt == 1) {
				        slow_forward();
					printf("slow forw\n");
			}
			else if(bananaHunt == 2) {
				        slow_forward_left();
					printf("forw left\n");
			}
			else if(bananaHunt == 3) {
				        slow_forward_right();
					printf("forw right\n");
			}
			else if(bananaHunt == 4) {
				        slow_left();
					printf("left\n");
			}
			else if(bananaHunt == 5) {
				        slow_right();
					printf("right\n");
			}
			else if(bananaHunt == 6) {
				        bananaDance();
					printf("banana dance\n");
			}


		}
		else if(rc_get_state()==PAUSED){
			rc_set_led(GREEN, OFF);
			rc_set_led(RED, ON);
		}

		usleep(100000);
	}
	pyThread = false;
	usleep(100000);
	
	Py_Finalize();
	usleep(100000);	
	rc_cleanup(); 
	return 0;
}

void *Ultrasonic_thread(void *vargp) {
	
	while(1) {

		cm_left = get_ultrasonic1_cm();
		cm_right = get_ultrasonic2_cm();
		usleep(100000);
	}
	return NULL;
	
}

void *Gyro_thread(void *vargp) {
	int gyro_cnt = 0;
	float gyro_sum = 0.0;

	while(1) {

		if(GyroDisable == false) {

			rc_read_gyro_data(&accel_data);
			GyroX = accel_data.gyro[2]*10;
			//printf("Gyro: %f\n", GyroX);

			if(gyro_cnt > 6) {

				//printf("AVG gyro: %f \n", (gyro_sum / gyro_cnt));
				if((gyro_sum / gyro_cnt) > 700 ||(gyro_sum / gyro_cnt) < -700) {
					spinning = true;
				}
				gyro_cnt = 0;
				gyro_sum = 0.0;
			}
			gyro_sum += GyroX;
			gyro_cnt++;
			usleep(80000);
		}	

		}
		return NULL;
	}

void *Accel_thread(void *vargp) {
	int accel_cnt = 0;
	float accel_sum = 0.0;
	
	while(1) {
		rc_read_accel_data(&accel_data);
		AccelX = floor(accel_data.accel[0]*10);
		AccelY = floor(accel_data.accel[1]*10);
		if(accel_cnt > 2)
		{
			if((accel_sum / accel_cnt) > 80) {
				//printf("\nAcc - X: %f, Y: %f", AccelX, AccelY);
				system("flite -t Ouch");
				collision = true;
			}
			accel_cnt = 0;
			accel_sum = 0.0;
		}
		accel_sum += AccelX;
		accel_cnt++;
		usleep(5000);
	}
	return NULL;
}


void forward() {
	rc_set_motor(1, 1.5);
	rc_set_motor(2, -1.5);
}

void slow_forward() {
	rc_set_motor(1, 0.25);
	rc_set_motor(2, -0.25);
}

void back() {
	rc_set_motor(1, -1.5);
	rc_set_motor(2, 1.5);
	usleep(500000);
}

void small_turn_right() {

	rc_set_motor(1, 1.5);
	rc_set_motor(2, -0.2);
	usleep(250000);
	forward();
}

void slow_forward_right() {
	rc_set_motor(1, 0.25);
	rc_set_motor(2, -0.1);
}

void small_turn_left() {

	rc_set_motor(1, 0.2);
	rc_set_motor(2, -1.5);
	usleep(250000);	
	forward();
}	

void slow_forward_left() {
	rc_set_motor(1, 0.1);
	rc_set_motor(2, -0.25);
}


void turn_right() {

	rc_set_motor(1, 0.2);
	rc_set_motor(2, 1.5);
	usleep(950000);
}

void slow_right() {
	rc_set_motor(1, 0.2);
	rc_set_motor(2, 0);
}

void turn_left() {

	rc_set_motor(1, -1.5);
	rc_set_motor(2, -0.2);
	usleep(950000);
}

void slow_left() {
	rc_set_motor(1, 0);
	rc_set_motor(2, -0.2);
}

void hold() {
	rc_set_motor(1, 0);
	rc_set_motor(2, 0);
	usleep(100000);
}

void bananaDance () {
	rc_set_motor(1, 1.5);
	rc_set_motor(2, 0);
	system("aplay minion.wav");
	sleep(20);
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


void *Python_thread(void *vargp) {
	while(pyThread) {

		PyRun_SimpleString("import sys; sys.path.append('/usr/lib/python2.7'); sys.path.append('/usr/lib/python2.7/plat-arm-linux-gnueabihf'); sys.path.append('/usr/lib/python2.7/lib-tk'); sys.path.append('/usr/lib/python2.7/lib-old'); sys.path.append('/usr/lib/python2.7/lib-dynload'); sys.path.append('/usr/local/lib/python2.7/dist-packages'); sys.path.append('/usr/local/lib/python2.7/dist-packages/Adafruit_BBIO-1.0.5-py2.7-linux-armv7l.egg'); sys.path.append('/usr/lib/python2.7/dist-packages');");

		PyObject *pName, *pModule;
		pName = PyString_FromString("test");
		pModule = PyImport_Import(pName);
	
		
		PyObject* myFunction = PyObject_GetAttrString(pModule,(char*)"openCV");
		PyObject* args = PyTuple_Pack(1,PyFloat_FromDouble(2.0));
		//PyObject* args = PyTuple_Pack(1,PyInt_FromLong(start));
		PyObject* myResult = PyObject_CallObject(myFunction, args);

		//double result = PyFloat_AsDouble(myResult);
		long result = PyInt_AsLong(myResult);

		bananaHunt = result;

		//printf("Result: %li\n", result);

	}

	return NULL;
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
