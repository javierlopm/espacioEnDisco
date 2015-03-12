#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define READ 0
#define WRITE 1

char *entrada="Si :) ";

main (int argc, char *argv[]) {

 int fd1[2];
 int fd2[2];
 int fd3[2];
 char salida[100];
 pipe(fd1);
 if (fork()==0) { 
   close(fd1[READ]);

   	while(1){
      sleep(2);
      write(fd1[WRITE], entrada, strlen(entrada) + 1);
      printf("Esperare...\n");
      }
   close(fd1[WRITE]);
 } else { 
   close(fd1[WRITE]); 

   while(1){
   	  printf("eh.... estoy esperando\n");
      read(fd1[READ], salida, 100);
      printf("Pase por el pipe?  %s", salida);
   }
   close(fd1[READ]);
 }

} 