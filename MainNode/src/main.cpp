#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include "listener.h"
#include "interlanguage.h"
#include <chrono>
#include <ctime>    

using namespace std;

//TODO: Main.h

uint32_t top_offset = 0;
uint32_t bottom_offset = 1;
uint32_t left_offset = 2;
uint32_t right_offset = 3;

uint32_t* pOut;

uint32_t old_PlayersMatrix[NPlayers*Fields];
uint32_t current_PlayersMatrix[NPlayers*Fields];


uint32_t top[NPlayers/2];
uint32_t bottom[NPlayers/2];

//TODO: Variables or Invariant to know where each has to score (pe: Team A top Team B bottom)
//		Important in isOffside check (*)

//Workarround. Only one is set because the other team will score on the other direction.
// 1 means top; 0 means bottom;
uint32_t dirTeamA = 1;

const uint32_t TeamA = 1;
const uint32_t TeamB = 0;

//TODO: Each call must have error handling
//TODO: Format and size of PlayersMatrix is not correct yet. Goalkeapers (?)

uint32_t max_top(uint32_t* old, uint32_t team){

	uint32_t result = 0;
	uint32_t i = 0;
	uint32_t Boffset = 0;

	switch (team){
	    case TeamA:
	    	//Prepare team A top array
	    	for(i=0;i < (NPlayers/2); i++) top[i] = old[(i*Fields)+top_offset];
	        break;

	    case TeamB:
	    	//Prepare team B top array
	    	Boffset = (NPlayers/2)*Fields;
	    	for(i=0;i < (NPlayers/2); i++) top[i] = old[Boffset+((i*Fields)+top_offset)];
	        break;
	}

	result = *max_element(&top[0],&top[NPlayers/2]);	
	return result;
}

uint32_t min_bottom(uint32_t* old, uint32_t team){

	uint32_t result = 0;
	uint32_t i = 0;
	uint32_t Boffset = 0;

	switch (team){
	    case TeamA:
	    	//Prepare team A top array
	    	for(i=0;i < (NPlayers/2); i++) bottom[i] = old[(i*Fields)+bottom_offset];
	        break;

	    case TeamB:
	    	//Prepare team B top array
	    	Boffset = (NPlayers/2)*Fields;
	    	for(i=0;i < (NPlayers/2); i++) bottom[i] = old[Boffset+((i*Fields)+bottom_offset)];
	        break;
	}

	result = *min_element(&bottom[0],&bottom[NPlayers/2]);	
	return result;
}

uint32_t isOffside(uint32_t* old, uint32_t* current){

	uint32_t result = 0;

	//Old == Passer && Current == Receiver
	uint32_t Passer_Team;
	uint32_t Receiver_Team;

	uint32_t Passer_top;
	uint32_t Passer_bottom;
	uint32_t Receiver_top;
	uint32_t Receiver_bottom;

	uint32_t old_Receiver_top;
	uint32_t old_Receiver_bottom;

	uint32_t min_Defender_bottom;
	uint32_t max_Defender_top;

	uint32_t isForward = 0;

	uint32_t id_receiver = current[0];
	uint32_t id_passer = old[0];

	//Get Passer and Receiver Teams. First Team A, Then Team B	
	if(id_passer < 44) Passer_Team = TeamA; else Passer_Team = TeamB;
	if(id_receiver < 44) Receiver_Team = TeamA; else Receiver_Team = TeamB; 

	//Only a pass between two players of the same team must be checked
	//If it's the same player means it is a autopass or simply he has run 
	//with the ball and in each ball touch trigger has been set, but it hasn't be checked
	if ((Passer_Team == Receiver_Team) && (id_passer != id_receiver)){

		//TODO: Capsule it on a function (?)
		//First: We must know if passing goes forward or backward

		Receiver_top 		=	current[id_receiver+top_offset];
		Receiver_bottom 	= 	current[id_receiver+bottom_offset];
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
			    	max_Defender_top = max_top(old,TeamB);
			    	if (old_Receiver_top  > max_Defender_top) result = 1;
			        break;

			    case TeamB:
			    	//TeamB Offense is Offside if Receiver_bottom is smaller than min(Defense_top)
			    	min_Defender_bottom = min_bottom(old,TeamA);
			    	if (old_Receiver_bottom  < min_Defender_bottom) result = 1;
			        break;
			}

		}

	}

	return result;

}

int main(int argc, char *argv[]) {

	int i;
	int MATLAB_status;
	int rsocket;
	int trigger;

	unsigned int Offside = 0;

	//Initialize MatrixPlayers
	for (i=0; i<88; i++) { old_PlayersMatrix[i] = 0; current_PlayersMatrix[i] = 0; }

	//Important the order
	MATLAB_status = iniMATLAB();
	rsocket = start_listening();


	printf("read_socket == %d and Mstatus == %d\n",rsocket,MATLAB_status);

	if(rsocket > 0 && MATLAB_status == 0){
		while(1){
			trigger = 0;
            //trigger = listen(rsocket);
			
			listen_img(rsocket);

			printf(".");
			printf("Trigger is: %d\n",trigger);

			//Process if there is Offside
			if(trigger == 1){

				//Profiling
				auto start = std::chrono::system_clock::now();

				pOut = getPlayersMatrix();

				for (i=0; i<88; i++) {
					//std::cout << "En el Main[i]: " << pOut[i] << std::endl;
					//Keep las PlayersMatrix in Old and get new in Current 
					old_PlayersMatrix[i] = current_PlayersMatrix[i];
					current_PlayersMatrix[i] = pOut[i];
				}

				Offside = isOffside(old_PlayersMatrix,current_PlayersMatrix); 
				if(Offside) printf("Rafa, no me jodas. Fuera de juego!\n");
				else printf("Sigan!\n");

				// Profiling
			    auto end = std::chrono::system_clock::now();

			    std::chrono::duration<double> elapsed_seconds = end-start;
			    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

			    std::cout << "Finished computation at " << std::ctime(&end_time)
			              << "Elapsed time: " << elapsed_seconds.count() << "s\n";

			}	
		}
	}
	
	printf("End\n");
	stop_listening(rsocket);

	//TODO: Problems when closing MATLAB session. Maybe kill with sigkill
	endMATLAB();
	exit(0);

	return 0;

}
