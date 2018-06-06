#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
//#define PORT 8080
int PORT=0;
char ipaddress[15]="\0";
void getDisk();
struct sysinfo systems;
struct datasys
{
	double up_day ; 
	double up_hour; 
	double up_min ;
	double up_sec;
	double total_ram;
	double free_ram;
	double proc_count;
	double min15_load;
	double cpu_util;
	double mem_percent;
	char hostname[256];
	char diskinfo[1024];
};
struct datasys datas;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *loadSystem (void *vargp)
{
	printf("Load System Thread started\n");
	datas.diskinfo[1024] = '\0';
   	 
	const long MIN = 60;
	const long HOUR = MIN*60;
	const long DAY = HOUR*24;
	const double MB = 1024*1024;
//	datas.hostname[256] = '\0';	
	gethostname(datas.hostname,255);
	//printf("loadsystem: %s\n",datas.diskinfo);

	while(1)
	{
		pthread_mutex_lock(&mutex);
			sysinfo(&systems);
				sleep(0.1);
		datas.up_day  = systems.uptime/DAY;
		datas.up_hour = (systems.uptime%DAY) / HOUR;
		datas.up_min = (systems.uptime%HOUR)/MIN;
		datas.up_sec = (systems.uptime%MIN);
		datas.total_ram = systems.totalram/MB;
		datas.free_ram = systems.freeram/MB;
		datas.mem_percent = (datas.total_ram - datas.free_ram )*100.0/datas.total_ram;
		datas.proc_count = systems.procs;
		datas.min15_load = systems.loads[1];
		pthread_mutex_unlock(&mutex);
	}
	
}
void *diskutil(void *args)
{
	printf("Disk Util Thread Started\n");
		FILE *p;
		pthread_mutex_lock(&mutex);
		p = popen("df -hT \"$@\" | grep -E '^/'", "r");
	
   		 if(!p) {
	      	 	 fprintf(stderr, "Error opening pipe.\n");
	      		 
	   	 }
		 int i = 0;
	  	  while(!feof(p)) {
	       		 //printf("%c", fgetc(p));
			 datas.diskinfo[i++] = fgetc(p);
	
	  	  
		  }
		  //datas.diskinfo[i] = '\0';
	
	 		//printf("%s\n",datas.diskinfo); 
		pthread_mutex_unlock(&mutex);

	    	if (pclose(p) == -1) {
	   	     fprintf(stderr," Error!\n");
	  	  }
		sleep(1);

}

void *cpuutil(void *args)
{
	printf("CPU Util Thread Started\n");
	long double a[4], b[4], loadavg;
	FILE *fp;
	char dump[50];
	//printf("cpuutil: %s\n",datas.diskinfo);


	while(1)
	{
        	fp = fopen("/proc/stat","r");
	        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
	        fclose(fp);
	        sleep(1);
	
	        fp = fopen("/proc/stat","r");
	        fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
	        fclose(fp);
		pthread_mutex_lock(&mutex);
	        datas.cpu_util = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
	        pthread_mutex_unlock(&mutex);
	}

}
void *printSystem(void *args)
{
	//printf("printsystem: %s\n",datas.diskinfo);

/*	while(1)
	{
		
		sleep(1);
		pthread_mutex_lock(&mutex);
		printf("Server Uptime = %3.0f days, %02.0f:%02.0f:%02.0f\n",datas.up_day,datas.up_hour,datas.up_min,datas.up_sec);
			printf("Total Ram = %.2f, usage:%.2f %% \n",datas.total_ram,datas.mem_percent);
		printf("Host: %s\n",datas.hostname);
		int n = 0;
		for(n = 0 ; n < sizeof(datas.diskinfo) ; n++)
			printf("%c",datas.diskinfo[n]);
		pthread_mutex_unlock(&mutex);

	}*/
struct sockaddr_in servaddr;
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
//  printf("Port:%d;\n",PORT);
  servaddr.sin_port = htons(PORT);
//  servaddr.sin_addr.s_addr = inet_addr("128.199.75.250");
printf("IP is %s\n",ipaddress);
servaddr.sin_addr.s_addr = inet_addr(ipaddress);

  while(1) {
printf("%f %f %f %f %f %f %f %f %f %f %s %s",datas.up_day,datas.up_hour,datas.up_min,datas.up_sec,datas.total_ram,datas.free_ram,datas.mem_percent,datas.proc_count,datas.min15_load,datas.cpu_util,datas.hostname,datas.diskinfo);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));
     
  
		pthread_mutex_lock(&mutex);	
		char txt[65535];
		const int len = sprintf(txt, "%3.0f*%02.0f*%02.0f*%02.0f*%.2f*%.2f*%.2f*%.2f*%.2f*%.2f*%s*%s",datas.up_day,datas.up_hour,datas.up_min,datas.up_sec,datas.total_ram,datas.free_ram,datas.mem_percent,datas.proc_count,datas.min15_load,datas.cpu_util,datas.hostname,datas.diskinfo);
	 	char *data_packet = txt; 
	
	    	send(sock , data_packet , strlen(data_packet) , 0 );
		pthread_mutex_unlock(&mutex);
		sleep(0.2);
  		close(sock);
	}
		

}

void flush()
{
    int c;
    while ((c = getchar()) != '\n')
        ;
}
int main() {

	printf("Insert IP Address of Server: ");
	scanf("%s",ipaddress);
	//puts(ipaddress);
	
	flush();
	printf("Insert port to open (MAX for now 3 Channel): ");
	scanf("%d",&PORT);
	flush();
	printf("Initializing Connection on port:%d\n",PORT);

	pthread_t thread1,thread2,thread3,thread4;
	pthread_create(&thread1,NULL,loadSystem,NULL);
	
	pthread_create(&thread4,NULL,diskutil,NULL);
	pthread_create(&thread3,NULL,cpuutil,NULL);
	pthread_create(&thread2,NULL,printSystem,NULL);

	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	pthread_join(thread3,NULL);
	pthread_join(thread4,NULL);

	






    	return 0;
}






