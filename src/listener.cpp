#include "listener.h"


//Globals
char buff[256];
char buffer[256];
char BALL_RECV[256];
char BUTTON_RECV[256];
char CAMERA_RECV[256];
 
int sock;
int port;
int status;

struct sockaddr_in servaddr;

  
int start_listening(int port, char *ADDR){

  sock = 0;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  status = 0;  

  if (sock == -1){
    printf("could not establish connection\n");
    return -1;
  }

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(ADDR);
  printf("ip %d && port %d -- %d\n",ADDR,port,htons(port));
  servaddr.sin_port = htons(port);
  status = connect(sock, (struct sockaddr*) &servaddr, sizeof(servaddr));

  if (status == 0) printf("connection is established successfully\n");
  else{
    printf("could not run the app\n");
    return -1;
  }

  return sock;

}

int stop_listening(int rsocket){

  close(rsocket);

}

int listen_pass(int rsocket){

  int result = -1;


  status = recv(rsocket,&buff[0],1,0);
  if (status > 0) result = atoi(&buff[0]);

  return result;

}

int ball_recv(int rsocket){

  int result = -1;

  //Only 1 byte is send so sizeof(buffer) cannot be the expected length; infintie loop
  
  status = recv(rsocket,&BALL_RECV[0],1,0);

  if (status > 0) result = atoi(&BALL_RECV[0]);

  return result;

}

int camera_recv(int rsocket){

  int result = -1;

  //Only 1 byte is send so sizeof(buffer) cannot be the expected length; infintie loop

  status = recv(rsocket,&CAMERA_RECV[0],1,0);

  if (status > 0) result = atoi(&CAMERA_RECV[0]);

  return result;

}

int button_recv(int rsocket){

  int result = -1;

  //Only 1 byte is send so sizeof(buffer) cannot be the expected length; infintie loop

  status = recv(rsocket,&BUTTON_RECV[0],1,0);

  if (status > 0) result = atoi(&BUTTON_RECV[0]);

  return result;

}

int listen_img(int socket){
		

  int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;

  char imagearray[10241],verify = '1';
  FILE *image;

  //Find the size of the image
  do{
  stat = read(socket, &size, sizeof(int));
  }while(stat<0);


  char buffer[] = "Got it";

  //Send our verification signal
  do{
  stat = write(socket, &buffer, sizeof(int));
  }while(stat<0);


  image = fopen("top.ppm", "w");

  if( image == NULL) {
  printf("Error has occurred. Image file could not be opened\n");
  return -1; }

  //Loop while we have not received the entire file yet

  int need_exit = 0;
  struct timeval timeout = {10,0};

  fd_set fds;
  int buffer_fd, buffer_out;

  while(recv_size < size) {

      FD_ZERO(&fds);
      FD_SET(socket,&fds);

      buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);

      if (buffer_fd < 0)
         printf("error: bad file descriptor set.\n");

      if (buffer_fd == 0)
         printf("error: buffer read timeout expired.\n");

      if (buffer_fd > 0)
      {
          do{
                 read_size = read(socket,imagearray, 10241);
              }while(read_size <0);


          //Write the currently read data into our image file
           write_size = fwrite(imagearray,1,read_size, image);

               if(read_size !=write_size) {
                   printf("error in read write\n");    }


               //Increment the total number of bytes read
               recv_size += read_size;
               packet_index++;

      }

  }

    fclose(image);

    return 1;
}

