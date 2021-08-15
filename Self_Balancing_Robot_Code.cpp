//compile using -lwiringPi and -lm extensions
#include <wiringPiI2C.h>
#include <math.h>
#include <iostream>
#include <wiringPi.h>
#include <unistd.h>
#include <iomanip>
#include <cstdlib>
#include <cmath>

using namespace std;

//global varriable from random code I adapted
int fd;

//prototypes
void pulseMotors(int, int, unsigned int, unsigned int);
int read_word_2c(int);
double dist(double, double);
double get_y_rotation(double, double, double);
double get_x_rotation(double, double, double);
double complimentary_filter(float, float, double, double, double);

int main()
{
  int acclX, acclY, acclZ;
  int gyroX, gyroY, gyroZ;
  double acclX_scaled, acclY_scaled, acclZ_scaled;
  double gyroX_scaled, gyroY_scaled, gyroZ_scaled;

	int pwmSpeed1, pwmSpeed2, countdown_0, test_speed, angle;
  float gyro_xout, complimentary;
	unsigned int usecs = 1, pulses = 1; //usecs: gap between motor pulses, pulses: number of pulses between each imu reading
 
 
  //setup i2c mpu6050 sensor
  fd = wiringPiI2CSetup (0x68);
  wiringPiI2CWriteReg8 (fd,0x6B,0x00);//disable sleep mode
  printf("set 0x6B=%X\n",wiringPiI2CReadReg8 (fd,0x6B));
  
  wiringPiI2CWriteReg8 (fd, 0x1B, 0x10); //set address 27 (gyro config) to 1000 deg/s 
	
  //setup gpio pin modes for motor controller
	wiringPiSetup();     
	pinMode(0, OUTPUT);
	pinMode(1, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);

	while(1) //main control loop,
	{
    		
    acclX = read_word_2c(0x3B);
    acclY = read_word_2c(0x3D);
    acclZ = read_word_2c(0x3F);    
    gyro_xout = read_word_2c(0x47)/32.8;
    
    acclX_scaled = acclX / 16384.0;
    acclY_scaled = acclY / 16384.0;
    acclZ_scaled = acclZ / 16384.0;
    
    complimentary = complimentary_filter(complimentary, gyro_xout, acclX_scaled, acclY_scaled, acclZ_scaled); //
    
    test_speed = 50 * cbrt(complimentary); //this is the scalar from degrees to motor speed, play around with this for better/different results
    
    cout<<test_speed<<endl;
    
    if(abs(test_speed) > 200) //kill the program if it falls over
    {
      cout<<"I fell over"<<endl;
      digitalWrite(0,0);
      digitalWrite(1,0);
      digitalWrite(3,0);     
      digitalWrite(4,0); 
      return 0;
    }
          
    pwmSpeed1 = test_speed;  
    pwmSpeed2 = test_speed;
		
		pulseMotors(pwmSpeed1, pwmSpeed2, usecs, pulses); //send motor power pulse to motors
	}	
 
	return 0;	
}


void pulseMotors(int motor1, int motor2, unsigned int usecs, unsigned int pulses)
{
	for(int x = 0; x < pulses; x++ )
	{
		if(motor1 > 0){                   //setup initial conditions before each pwm cycle
			//cout<<"+motor1 true"<<endl;
      digitalWrite(0,1);
      digitalWrite(1,0);
			
		}else if(motor1 < 0){
			//cout<<"-motor1 true"<<endl;
      digitalWrite(0,0);
      digitalWrite(1,1);	
				
		}else{
			//cout<<"motor1 false"<<endl;	
      digitalWrite(0,0);
      digitalWrite(1,0);	
		}
		
		if(motor2 > 0){                
			//cout<<"+motor1 true"<<endl;
      digitalWrite(3,1);
      digitalWrite(4,0);
			
		}else if(motor2 < 0){
			//cout<<"-motor1 true"<<endl;
      digitalWrite(3,0);
      digitalWrite(4,1);	
				
		}else{
			//cout<<"motor1 false"<<endl;	
      digitalWrite(3,0);
      digitalWrite(4,0);	
		}
		
		for(int i = 1 ; i < 100 ; i++) //subdivision of each pulse
		{
			//motor 1
			if(motor1 >= 0) //positive (or zero)
			{
				if(i == motor1){
					//cout<<"+motor1 false"<<endl;
          digitalWrite(0,0);				
				}

			}else{   //negative
			
				if(i == abs(motor1)){
					//cout<<"-motor1 false"<<endl;
          digitalWrite(1,0);
				}
			}
					
			//motor 2
			if(motor2 >= 0) //positive (or zero)
			{
				if(i == motor2){
					//cout<<"+motor2 false"<<endl;
          digitalWrite(3,0);				
				}

			}else{   //negative
			
				if(i == abs(motor2)){
					//cout<<"-motor2 false"<<endl;
          digitalWrite(4,0);
				}
			}
			usleep(usecs);		
		}	
	}			
}

int read_word_2c(int addr)
{
  int val;
  val = wiringPiI2CReadReg8(fd, addr);
  val = val << 8;
  val += wiringPiI2CReadReg8(fd, addr+1);
  if (val >= 0x8000)
  {
    val = -(65536 - val);
  }
  return val;
}

double dist(double a, double b)
{
  return sqrt((a*a) + (b*b));
}

double get_y_rotation(double x, double y, double z)
{
  double radians;
  radians = atan2(x, dist(y, z));
  return -(radians * (180.0 / M_PI));
}

double get_x_rotation(double x, double y, double z)
{
  double radians;
  radians = atan2(y, dist(x, z));
  return (radians * (180.0 / M_PI));
}

double complimentary_filter(float complimentary, float gyro_xout, double acclX_scaled, double acclY_scaled, double acclZ_scaled)
{
  return 0.98*(complimentary + ((gyro_xout-4.873)*0.104) + (0.02* get_x_rotation(acclX_scaled, acclY_scaled, acclZ_scaled))); 
}
