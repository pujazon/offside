#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include "listener.h"
#include "interlanguage.h"
#include "speaker.h"
#include <chrono>
#include <chrono>
#include <ctime>
   

using namespace std;

uint32_t reciver;

uint32_t top_offset = 0;
uint32_t bottom_offset = 1;
uint32_t left_offset = 2;
uint32_t right_offset = 3;

uint32_t old_PlayersMatrix[N];
uint32_t current_PlayersMatrix[N];


uint32_t top[NPlayers/2];
uint32_t bottom[NPlayers/2];


uint32_t dirTeamA = 1;

const uint32_t TeamA = 1;
const uint32_t TeamB = 0;


uint32_t max_top(uint32_t* old, uint32_t team){

	uint32_t result = 0;
	uint32_t i = 0;
	uint32_t Boffset = 0;

	switch (team){
	    case TeamA:
	    	//Prepare team A top array
	    	for(i=0;i < (NPlayers/2); i++) top[i] = old[1+(i*Fields)+top_offset];
	        break;

	    case TeamB:
	    	//Prepare team B top array
	    	Boffset = (NPlayers/2)*Fields;
	    	for(i=0;i < (NPlayers/2); i++) top[i] = old[1+Boffset+((i*Fields)+top_offset)];
	        break;
	}

	result = *max_element(&top[0],&top[NPlayers/2]);	
	return result;
}


uint32_t max_bottom(uint32_t* old, uint32_t team){

	uint32_t result = 0;
	uint32_t i = 0;
	uint32_t Boffset = 0;

	switch (team){
	    case TeamA:
	    	//Prepare team A top array
	    	for(i=0;i < (NPlayers/2); i++) bottom[i] = old[1+(i*Fields)+bottom_offset];
	        break;

	    case TeamB:
	    	//Prepare team B top array
	    	Boffset = (NPlayers/2)*Fields;
	    	for(i=0;i < (NPlayers/2); i++) bottom[i] = old[1+Boffset+((i*Fields)+bottom_offset)];
	        break;
	}

	result = *max_element(&bottom[0],&bottom[NPlayers/2]);	
	return result;
}

uint32_t min_bottom(uint32_t* old, uint32_t team){

	uint32_t result = 0;
	uint32_t i = 0;
	uint32_t Boffset = 0;

	switch (team){
	    case TeamA:
	    	//Prepare team A top array
	    	for(i=0;i < (NPlayers/2); i++) bottom[i] = old[1+(i*Fields)+bottom_offset];
	        break;

	    case TeamB:
	    	//Prepare team B top array
	    	Boffset = (NPlayers/2)*Fields;
	    	for(i=0;i < (NPlayers/2); i++) bottom[i] = old[1+Boffset+((i*Fields)+bottom_offset)];
	        break;
	}

	result = *min_element(&bottom[0],&bottom[NPlayers/2]);	
	return result;
}


uint32_t min_top(uint32_t* old, uint32_t team){

	uint32_t result = 0;
	uint32_t i = 0;
	uint32_t Boffset = 0;

	switch (team){
	    case TeamA:
	    	//Prepare team A top array
	    	for(i=0;i < (NPlayers/2); i++) top[i] = old[1+(i*Fields)+top_offset];
	        break;

	    case TeamB:
	    	//Prepare team B top array
	    	Boffset = (NPlayers/2)*Fields;
	    	for(i=0;i < (NPlayers/2); i++) top[i] = old[1+Boffset+((i*Fields)+top_offset)];
	        break;
	}

	result = *min_element(&top[0],&top[NPlayers/2]);	
	return result;
}

uint32_t isOffside(uint32_t* old, uint32_t* current){

	uint32_t result = 0;

	uint32_t Passer_Team;
	uint32_t Receiver_Team;

	uint32_t Passer_top;
	uint32_t Passer_bottom;
	uint32_t Receiver_top;
	uint32_t Receiver_bottom;

	uint32_t min_Defender_bottom;
	uint32_t max_Defender_top;

	uint32_t isForward = 0;

	uint32_t id_receiver = current[0];
	uint32_t id_passer = old[0];

	//Get Passer and Receiver Teams. First Team A, Then Team B	
	if(id_passer < 1+((NPlayers*Fields)/2)) Passer_Team = TeamA; else Passer_Team = TeamB;
	if(id_receiver < 1+((NPlayers*Fields)/2)) Receiver_Team = TeamA; else Receiver_Team = TeamB; 

	//Only a pass between two players of the same team must be checked
	//If it's the same player means it is a autopass or simply he has run 
	//with the ball and in each ball touch trigger has been set, but it hasn't be checked
	if ((Passer_Team == Receiver_Team) && (id_passer != id_receiver)){

	
		//First: We must know if passing goes forward or backward
		//We work with Old positions always because they are the relevant ones

		Receiver_top 		=	old[id_receiver+top_offset];
		Receiver_bottom 	= 	old[id_receiver+bottom_offset];
		Passer_top			=	old[id_passer+top_offset];
		Passer_bottom	 	= 	old[id_passer+bottom_offset];

		switch (Passer_Team){
		    case TeamA:
		    	//TeamA attacks top so if Reciver.top bigger than Sender.top is forward
		        if (Receiver_top > Passer_top) isForward = 1;
		        break;

		    case TeamB:
				//Team B attacks bottom so if Reciver bottom is smaller than Sender bottom is forward
				if(Receiver_bottom < Passer_bottom) isForward = 1;
		        break;
		}


		if(isForward){
			//Now we must check Reciver top or bottom with All Defense team top or bottom 
			//depending direction 	
			//We must work with old Reciver position because it's the significant one		
			//PlayersMatrix each player always have the same 

			switch (Receiver_Team){
			    case TeamA:
			    	//TeamA Offense is Offside if Receiver_top is bigger than max(Defense_top)
			    	max_Defender_bottom = max_bottom(old,TeamB);
			    	if (Receiver_bottom  > max_Defender_bottom) result = 1;
			        break;

			    case TeamB:
			    	//TeamB Offense is Offside if Receiver_bottom is smaller than min(Defense_top)
			    	min_Defender_top = min_top(old,TeamA);
			    	if (Receiver_top  < min_Defender_top) result = 1;
			        break;
			}

		}

	}

	return result;

}

int main(int argc, char *argv[]) {

	int i;
	int MATLAB_status;
	int Camera_socket;
	int Button_socket;
	int Ball_socket;
	int pass_trigger;
	int track_trigger;

	unsigned int Offside = 0;

	//Initialize MatrixPlayers
	for (i=0; i<(1+NPlayers*Fields); i++) { old_PlayersMatrix[i] = 0; current_PlayersMatrix[i] = 0; }

	//Important the order
	MATLAB_status = iniMATLAB();
	Camera_socket = start_listening(PORTA,"192.168.1.34");
	Ball_socket = start_listening(PORTB,"192.168.1.33");
	Button_socket = start_listening(PORTC,"192.168.1.33");

	printf("Camera Socket == %d  Button Socket == %d  Ball_socket == %d and Mstatus == %d\n",Camera_socket,Button_socket,Ball_socket,MATLAB_status);

	if(Ball_socket > 0 && Camera_socket > 0 && Button_socket > 0 && MATLAB_status == 0){
		
		printf("Ini()...\n");

		printf("Req camera\n");	
		req_camera(Camera_socket);
		listen_img(Camera_socket);

		getPlayersMatrix(1,current_PlayersMatrix);	
		printf("Getd\n");
		for (i=0; i<N; i++) {
			old_PlayersMatrix[i] = current_PlayersMatrix[i];
			current_PlayersMatrix[i] = pOut[i];
		}
	
		reciver = 0;
	
		printf("Ini 3 seconds synchronizing whole system\n");
		sleep(3);	

		while(1){

			printf("Req camera\n");	
			req_camera(Camera_socket);
			listen_img(Camera_socket);
	
			printf("Req Ball\n");	
			req_ball(Ball_socket);
			pass_trigger = listen_pass(Ball_socket);
			
			printf("Req Button\n");	
			req_button(Button_socket);
			track_trigger = listen_pass(Button_socket);
		        printf("Button %d \n",track_trigger);	
						
			if(track_trigger == 1){							
				printf("Tracking!\n");
				
				//Profiling
				//auto start = std::chrono::system_clock::now();
				getPlayersMatrix(0,current_PlayersMatrix);

			
				for (i=0; i<N; i++) {
					old_PlayersMatrix[i] = current_PlayersMatrix[i];
					current_PlayersMatrix[i] = pOut[i];
				}

					//auto end = std::chrono::system_clock::now();
			    	//std::chrono::duration<double> elapsed_seconds = end-start;
			    	//std::time_t end_time = std::chrono::system_clock::to_time_t(end);
			    	//std::cout << "Tracking(): " << elapsed_seconds.count() << "s\n";
			}
				
			if(pass_trigger == 1){				
	
					printf("Pass recived!!!!!! Offside ? == %d\n",pass_trigger);	

					
							
					printf("Rafa a \n");	
					req_photo(Camera_socket);
					req_camera(Camera_socket);
					listen_img(Camera_socket);

					getPlayersMatrix(0,current_PlayersMatrix);

					for (i=0; i<N; i++) {
						old_PlayersMatrix[i] = current_PlayersMatrix[i];
						current_PlayersMatrix[i] = pOut[i];
					}

					// Profiling
					//auto start = std::chrono::system_clock::now();
					
					Offside = isOffside(old_PlayersMatrix,current_PlayersMatrix); 
					
					//auto end = std::chrono::system_clock::now();				
					//std::chrono::duration<double> elapsed_seconds = end-start;
					//std::time_t end_time = std::chrono::system_clock::to_time_t(end);
					//std::cout << "isOffside(): " << elapsed_seconds.count() << "s\n";
					
					if(Offside) printf("Fuera de juego!\n");
					else printf("Sigan!\n");
			}
				
		}
	}
	
	printf("End\n");
	
	//TODO: Problems when closing MATLAB session. Maybe kill with sigkill
	stop_listening(Ball_socket);
	stop_listening(Camera_socket);
	endMATLAB();
	exit(0);

	return 0;

}

