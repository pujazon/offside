#include <stdio.h>
#include <wiringPi.h>

int main(int argc, char** argv)
{
    // Intialize the wiringPi Library
    wiringPiSetup();

    const int INPUT_PIN = 1;
	
    // Read input on this pin
    pinMode(INPUT_PIN, INPUT);
//    pullUpDnControl(INPUT_PIN,PUD_UP);
    while(true)
    {

        // As soon as we dedect an input, log and quit.
        if(digitalRead(INPUT_PIN) == LOW)
		{
			printf("Button is pressed!\n");	
			break;
		}
    }

    // Exit program
    return 0;
}
