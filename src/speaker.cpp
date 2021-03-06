#include "speaker.h"
#include <stdio.h>
#include <stdlib.h>

char PASS[256];
char BUTTON[256];
char BALL_REQ[256];
char CAMERA_REQ[256];
char BUTTON_REQ[256];


int start_speaking(int port){

	struct sockaddr_in server;
	char in[1];
	int sock = 0;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1) fprintf(stderr, "failed\n");
	else printf("Sock %d connection is establisshed\n",sock);

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	int status = bind(sock, (struct sockaddr*) &server, sizeof(server));

	if (status == 0) printf("Connection completed\n");
	else printf("Problem is encountered %d \n",status);

	return sock;

}

int meeting(int sock){

	int status = listen(sock, 1);
	if (status == 0) printf("App is ready to work\n");
	else
	{
		printf("Connection is failed\n");
		return -1;
	}

	struct sockaddr_in client = { 0 };
	int sclient = 0;
	unsigned int len = sizeof(client);


    int childSocket = accept(sock, (struct sockaddr*) &client, &len);
    if (childSocket == -1)
    {
		printf("cannot accept connection\n");
		close(sock);
		return -1; 
    }

    return childSocket;
}

int stop_speaking(int sock){
  
  close(sock);
  return 0;
  
}

int speak_pass(int ssocket, char value){

	PASS[0] =  value;
	printf("speak() == %c\n",PASS[0]);

	send(ssocket,&PASS[0],1,0);

	return 0;
}

int speak_button(int ssock, char value){

	BUTTON[0] =  value;
	printf("Button() == %c\n",BUTTON[0]);

	send(ssock,&BUTTON[0],1,0);

	return 0;
}

int req_ball(int ssocket){

	BALL_REQ[0] =  '1';
	printf("BALL_REQ() == %c\n",BALL_REQ[0]);

	send(ssocket,&BALL_REQ[0],1,0);

	return 0;
}

int req_camera(int ssocket){

	CAMERA_REQ[0] =  '1';
	printf("CAMERA_REQ() == %c\n",CAMERA_REQ[0]);

	send(ssocket,&CAMERA_REQ[0],1,0);

	return 0;
}

int req_photo(int ssocket){

	CAMERA_REQ[0] =  '2';
	printf("PHOTO_REQ() == %c\n",CAMERA_REQ[0]);

	send(ssocket,&CAMERA_REQ[0],1,0);

	return 0;
}

int req_button(int ssocket){

	BUTTON_REQ[0] =  '1';
	printf("CAMERA_REQ() == %c\n",BUTTON_REQ[0]);

	send(ssocket,&BUTTON_REQ[0],1,0);

	return 0;
}

int speak_img(int socket){

   FILE *picture;
   int size, read_size, stat, packet_index;
   char send_buffer[10240], read_buffer[256];
   packet_index = 1;

   picture = fopen("top.ppm", "r");

   if(picture == NULL) {
        printf("Error Opening Image File"); } 

   fseek(picture, 0, SEEK_END);
   size = ftell(picture);
   fseek(picture, 0, SEEK_SET);

   //Send Picture Size
   write(socket, (void *)&size, sizeof(int));

   //Send Picture as Byte Array
   printf("Sending Picture as Byte Array\n");

   do { //Read while we get errors that are due to signals.
      stat=read(socket, &read_buffer , 255);
   } while (stat < 0);


   while(!feof(picture)) {

      read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

      //Send data through our socket 
      do{
        stat = write(socket, send_buffer, read_size);  
      }while (stat < 0);

      packet_index++;  
	printf("Send img correctly()\n");
      //Zero out our send buffer
      bzero(send_buffer, sizeof(send_buffer));
     }
    }
