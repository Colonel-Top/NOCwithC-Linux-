//
// Created by proms on 6/6/2018.
//

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Winsock2.h>
#include <windows.h>
#include <unistd.h>
#include <ws2tcpip.h>
#include <io.h>
#include <conio.h>

int portable = 8080;
#pragma comment(lib,"ws2_32.lib") //Winsock Library
int mode = 0;
struct datasys
{ 
    char * up_day ; 
    char * up_hour; 
    char * up_min ;
    char * up_sec;
    char * total_ram;
    char * free_ram;
    char * proc_count;
    char * min15_load;
    char * cpu_util;
    char * mem_percent;
    char *hostname;
    char *diskinfo;
};
struct datasys datas[4];
int allow[4] = {0};
void *socketThread(void *args)
{
    int port = *((int *)args);
    printf("Thread Port[%d]\n",port);
    int index = port-portable;

    WSADATA wsa;
    SOCKET s , new_socket;
    struct sockaddr_in server , client;
    int c;
    char *message;

    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(-1);
    }

    printf("Initialised.\n");

    //Create a socket
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }

    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );

    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    puts("Bind done");

    //Listen to incoming connections
    listen(s , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");

    c = sizeof(struct sockaddr_in);

    while( (new_socket = accept(s , (struct sockaddr *)&client, &c)) != INVALID_SOCKET )
    {
        //puts("Connection accepted\n");
        allow[index] = 1;
        //puts(sockaddr.sin_addr);


        //Reply to the client
        message = "Socket connected.\n";
        send(new_socket , message , strlen(message) , 0);
        char server_reply[65535];
        if( recv(new_socket, server_reply , 6000 , 0) < 0)
        {
            puts("recv failed");
              allow[index] = 0;
              // sscanf(server_reply,"0 0 0 0 0 0 0 0 0 0 0 0",&datas[index].up_day,&datas[index].up_hour,&datas[index].up_min,&datas[index].up_sec,&datas[index].total_ram,&datas[index].free_ram,&datas[index].mem_percent,&datas[index].proc_count,&datas[index].min15_load,&datas[index].cpu_util,datas[index].hostname,datas[index].diskinfo);
        }

        //puts(server_reply);
        //sscanf(server_reply,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s %s",&datas[index].up_day,&datas[index].up_hour,&datas[index].up_min,&datas[index].up_sec,&datas[index].total_ram,&datas[index].free_ram,&datas[index].mem_percent,&datas[index].proc_count,&datas[index].min15_load,&datas[index].cpu_util,datas[index].hostname,datas[index].diskinfo);
        char *up_day;
        char *up_hour;
        char *up_min;
        char *up_sec;
        char *total_ram;
        char *free_ram;
        char *mem_percent;
        char *proc_count;
        char *min15_load;
        char *cpu_util;
        char *hostname;
        char *diskinfo;

        // char *oldstring = malloc(sizeof(server_reply));
        // strcpy(oldstr,str);

        datas[index].up_day=strtok(server_reply,"*"); 
        datas[index].up_hour=strtok(NULL,"*"); 
        datas[index].up_min=strtok(NULL,"*"); 
        datas[index].up_sec=strtok(NULL,"*"); 
        datas[index].total_ram=strtok(NULL,"*"); 
        datas[index].free_ram=strtok(NULL,"*"); 
        datas[index].mem_percent=strtok(NULL,"*"); 
        datas[index].proc_count=strtok(NULL,"*"); 
        datas[index].min15_load=strtok(NULL,"*"); 
        datas[index].cpu_util=strtok(NULL,"*"); 
        datas[index].hostname=strtok(NULL,"*");
        datas[index].diskinfo=strtok(NULL,"*");
        
        // puts(datas[index].up_sec);
        // datas[index].up_day=up_day; 
        // datas[index].up_hour=up_hour; 
        // datas[index].up_min=up_min; 
        // datas[index].up_sec=up_sec; 
        // datas[index].total_ram=total_ram; 
        // datas[index].free_ram=free_ram; 
        // datas[index].mem_percent=mem_percent; 
        // datas[index].proc_count=proc_count; 
        // datas[index].min15_load=min15_load; 
        // datas[index].cpu_util=cpu_util; 
        // datas[index].hostname=hostname;
        // strcpy(datas[index].up_day,up_day); 
        // strcpy(datas[index].up_hour,up_hour); 
        // strcpy(datas[index].up_min,up_min); 
        // strcpy(datas[index].up_sec,up_sec); 
        // strcpy(datas[index].total_ram,total_ram); 
        // strcpy(datas[index].free_ram,free_ram); 
        // strcpy(datas[index].mem_percent,mem_percent); 
        // strcpy(datas[index].proc_count,proc_count); 
        // strcpy(datas[index].min15_load,min15_load); 
        // strcpy(datas[index].cpu_util,cpu_util); 
        // strcpy(datas[index].hostname,hostname);
        // puts(datas[index].hostname);
    }

    if (new_socket == INVALID_SOCKET)
    {
        printf("accept failed with error code : %d" , WSAGetLastError());
        exit(-1);
    }

    closesocket(s);
    WSACleanup();
}
void *menuThread(void *args)
{
    printf("Menu Thread Started\n");
 
    while(1)
    {
        // puts(datas[0].up_sec);
        // printf("This is Hostname: %s\n",datas[mode].hostname);
        if(allow[mode] == 0)
            continue;
        // printf("%.0f %2.0f %2.0f %2.0f %f %f %f %f %f %f %s %s\n",datas[mode].up_day,datas[mode].up_hour,datas[mode].up_min,datas[mode].up_sec,datas[mode].total_ram,datas[mode].free_ram,datas[mode].mem_percent,datas[mode].proc_count,datas[mode].min15_load,datas[mode].cpu_util,datas[mode].hostname,datas[mode].diskinfo);
         system("cls");
         sleep(0.5);
         printf("Available: ");
         for(int i=0; i<3; i++)
         {
            if(allow[i] ==1)
                printf("Client[%d] \n",i+1);
         }
         puts("\n----------------\n");
         printf("Current Channel:[%d]\n--------\n",mode+1 );
         if(allow[mode] !=0)
         {

         printf("Client_Name: %s\n\n",datas[mode].hostname);
         printf("PORT: %d\n\n",portable);
         printf("Up Time: %s days,%shrs:%smins:%ssecs\n\n",datas[mode].up_day,datas[mode].up_hour,datas[mode].up_min,datas[mode].up_sec);
         printf("RAM: Total -> %s Usage -> %s Free -> %s  ",datas[mode].total_ram,datas[mode].mem_percent,datas[mode].free_ram);
         printf("with %s process\n\n",datas[mode].proc_count);
         printf("CPU: Load(in 15 mins) -> %s Utilization@ -> %s%%\n\n",datas[mode].min15_load,datas[mode].cpu_util);
         printf("DISK Information:\n\n %s",datas[mode].diskinfo);  
         }
         
         
    }
}
void *keyAction(void *args)
{
    char st;
    while(1)
    {

        st = getch();
        if(st == '1')
            mode = 0;
        else if (st == '2')
            mode = 1;
        else if(st == '3')
            mode = 2;
        else if(st == 'q')
            exit(-1);
    }
}
int main()
{
  
    printf("Please Insert Port Begin: ");
    scanf("%d",&portable);

    pthread_t thread[3];
    for(int n = 0 ; n < 3 ; n++)
    {
        int *i = malloc(sizeof(*i));
        *i = portable+ n;
        pthread_create(&thread[n],NULL,socketThread,i);
    }
    pthread_t threadMenu,keyact;
       pthread_create(&threadMenu,NULL,menuThread,NULL);
        pthread_create(&keyact,NULL,keyAction,NULL);
    for(int n = 0 ; n  < 3 ; n++){
           pthread_join(thread[n],NULL);
       }
       
       pthread_join(threadMenu,NULL);
       pthread_join(keyact,NULL);
}
void printinfo()
{


}