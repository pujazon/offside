#include "mpu6050.h"

int read_word_2c(int addr)	{
	int val;
	val = wiringPiI2CReadReg8(fd, addr);
	val = val << 8;
	val += wiringPiI2CReadReg8(fd, addr+1);
	if (val >= 0x8000)
	val = -(65536 - val);
	 
	return val;
}
 
double dist(double a, double b)	{
	return sqrt((a*a) + (b*b));
}
 
double get_y_rotation(double x, double y, double z)	{
	double radians;
	radians = atan2(x, dist(y, z));
	return -(radians * (180.0 / M_PI));
}
 
double get_x_rotation(double x, double y, double z)	{
	double radians;
	radians = atan2(y, dist(x, z));
	return (radians * (180.0 / M_PI));
}

//(1) Double abs
double dabs(double a, double b){
	if (a > b) return a-b;
	else return b-a;
}
 
int mpu6050(){

	thX = 0.6;	
	thY = 0.6;
	thZ = 0.6;

	internal_trigger = 10;

	unsigned int iter = 0;

	fd = wiringPiI2CSetup (0x68);
	wiringPiI2CWriteReg8 (fd,0x6B,0x00);//disable sleep mode
	printf("set 0x6B=%X\n",wiringPiI2CReadReg8 (fd,0x6B));
	ini = 1;

	while(1) {
			
		tacclX = read_word_2c(0x3B);
		tacclY = read_word_2c(0x3D);
		tacclZ = read_word_2c(0x3F);


		//(2) Take care that each 100 ms is triggered the MPU6050 values.
		//This menas that could be the situation of a player passing the ball
		//So this is 1 system trigger but on the action there are 10 mpu6050.c 
		//triggers and could be 0 0 1 0 0 1 0 (1 means real trigger)
		//and only should be one
		if (ini){
		 
			acc[0] = tacclX / 16384.0;
			acc[1] = tacclY / 16384.0;
			acc[2] = tacclZ / 16384.0;

			ini = 0;
		}

		else{
			acclX = tacclX / 16384.0;
			acclY = tacclY / 16384.0;
			acclZ = tacclZ / 16384.0;

				//(1)REcive pass could mean: or deacresing acceleration threshold (if you stop the bal when it has high speed). So absoulte difference value must be checked
			//TODO: If reception is get when ball is stopping there is no accelearation trigger so we won't detect it
			   if (internal_trigger > 10 &&
					   (dabs(acc[0],acclX) >= thX ||
					   dabs(acc[1],acclY) >= thY ||
					   dabs(acc[2],acclZ) >= thZ)){
			
					internal_trigger = 0;	       

				printf("My acclX_scaled: %f\n", acclX);
				printf("My acclY_scaled: %f\n", acclY);
				printf("My acclZ_scaled: %f\n", acclZ);
			}

			acc[0] = tacclX / 16384.0;
			acc[1] = tacclY / 16384.0;
			acc[2] = tacclZ / 16384.0;

		}

		printf("Inter %d\n", iter);

		iter++;
		internal_trigger++;

		delay(100);
	}
	
	return 0;
	
}
