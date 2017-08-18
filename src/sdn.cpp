#include<fstream>
#include<iostream>
#include<fstream>
#include<algorithm>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include<netdb.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<signal.h>
#include<errno.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<stdio.h>
#include<string>
#include<vector>
#include<sys/time.h>

using namespace std;

/**
 * @naveenmu_assignment3
 * @author  Naveen Muralidhar Prakash <naveenmu@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */

vector<int>sockets_connected;
vector<int>sockets_received;
struct timer
{

uint16_t id;
struct timeval interval;

};

struct timer t;
vector <struct timer> mul_timer;

struct controlmessage
{

uint32_t ip;
uint8_t con_code;
uint8_t response_time;
uint16_t payload_length;
};


struct forwarding_table
{
uint16_t dest;
uint16_t next_hop;
uint16_t cost;
};

vector<struct forwarding_table>vforward;

struct forwarding_table fwt;

struct controlresponse
{
uint32_t ip;
uint8_t con_code;
uint8_t response_code;
uint16_t payload_length;

};

struct controlpayload
{
uint16_t router_id;
uint16_t router_port;
uint16_t data_port;
uint16_t cost;
uint32_t router_ip;

};
uint16_t nodes,interval;

vector<struct controlpayload> vcp;

struct routingtable
{
uint16_t router_id;
uint32_t source_ip;
uint32_t router_ip;
int next_hop;
int neighbor_status;
uint16_t cost;

uint16_t router_port;
int inc_time;

};

struct dvbyid
{
uint16_t id;
uint32_t router_ip;
vector<struct routingtable> vrt;

};


struct controlmessage *cm;
struct controlresponse *cr;
struct controlpayload cp;
struct routingtable rt;  // because id starts from 1...5
struct forwarding_table *fw;
struct dvbyid dv;

vector<struct dvbyid>vdv;
ofstream myfile;
char file[4];

struct neighbor
{
uint16_t id;
uint16_t cost;
};

vector< struct neighbor>vneighbor;

struct linkcost
{
uint16_t id;
uint16_t cost;
};

struct linkcost li;

vector<struct linkcost>vlink;

vector<int> vcontrol;
vector<int>vdata;


struct sendfile
{
uint32_t dest_ip;
uint8_t ttl;
uint8_t tid;
uint16_t seq_num;
char filename[20];
};

struct sendfile sf;



struct datapacket

{

uint32_t dest_ip;
uint8_t tid;
uint8_t ttl;
uint16_t seq_num;
uint32_t padding;
unsigned char payload[1024];


};

vector<uint8_t>existing; 

struct send_stats
{

uint8_t tid;
uint8_t ttl;

vector<uint16_t>seq;


};

struct send_stats st;

vector<struct send_stats> vsend_stats;

struct datapacket dp;

	
	class DV
	{
	unsigned char buffer1[2048];
	FILE *fp,*fpwrite;	
	struct sockaddr_in udpaddr;
	struct addrinfo hints, *servinfo,*p;
	int addr,bytesread,current_position;
	uint16_t my_id,pad;
	uint32_t my_ip;
	long totalbytes;
	char filestore[8],tidstore[4];	
	uint16_t my_udp_port,my_data_port,hop;
	unsigned char *b1;
	
	int m,n,yes,neighbor,z,mytime,fragments,fragments_read;
	unsigned int x,control_port,count;
	fd_set master_list,watch_list;
	uint16_t uid,ucost,next_hop_port;

	long lsize;		
	int present,ds,send_sock,selret,opt,b,control_socket,udp_socket,data_socket,fdmax,sock_index,fdaccept,next_hop_socket,data_sock1,data_sock2;
        struct sockaddr_in data_addr,udp_addr,control_addr,client_addr;
	uint32_t next_hop_ip;     
	struct sockaddr_in connect_next_hop;
	unsigned char file_data[1024],sendbuffer[1036],data_payload[1024];	
	uint16_t seqnum,source_seqnum;
	uint32_t padfin;	
	unsigned int sq;
	uint8_t stats_tid;
	int ith;	
	unsigned char temp[1036],penultimate[1036],last[1036];
	public:
	DV()
	{
	memset(temp,'\0',1036);
	memset(penultimate,'\0',sizeof(penultimate));
	memset(last,'\0',sizeof(penultimate));
	fpwrite=NULL;
	b1=new unsigned char[1024];
	ith=0,padfin=1<<31,yes=1,neighbor=0,z=0,mytime=0,udp_socket=-1,source_seqnum=0;
	}


	int findpos(uint16_t n)   // find pos of neighbor in my local copy of dv
		 
	{
	int i;
	for( i=0;i<vdv.size();i++)
	{

	if(n==vdv[i].id)
	break;
	}

	return i;

	}


	int check_stats(uint8_t tid)
	{

	int i; bool flag=false;

	for(i=0;i<vsend_stats.size();i++)
	{

	if(tid==vsend_stats[i].tid) 
	{

	flag=true;	 break;

		}}

	if(flag==true) return i ;  //position
	else if(flag==false)return -1;
	}		

	//Bellman Ford Algorithm to calculate the minimum cost path to all the routers in the network
	void bellman_ford()

	{
		int i,j;
	uint16_t link_cost,next_hop; 
	unsigned long int min_cost;
	int mypos=findpos(my_id);    //Position of my Router-ID


	for( j=0;j<vdv[mypos].vrt.size();j++)
	{

	link_cost=vlink[j].cost;   //linkcost
	next_hop=vlink[j].id;
	min_cost=vlink[j].cost;
	if(vdv[mypos].vrt[j].router_id!=my_id)   // find cost to all the routers except myself 
	{
        
	for(int n=0;n<vneighbor.size();n++)   //find position of neighbors
	{

	int npos=findpos(vneighbor[n].id);
	unsigned long int cost=vneighbor[n].cost+vdv[npos].vrt[j].cost;

	if( cost< min_cost )
	{

		min_cost=cost;

		next_hop=vneighbor[n].id;
	
	}
}



if(min_cost<=65535)
{
vdv[mypos].vrt[j].cost=uint16_t(min_cost);
vdv[mypos].vrt[j].next_hop=next_hop;
}
else
{
vdv[mypos].vrt[j].cost=65535;
vdv[mypos].vrt[j].next_hop=65535;


}

}

}

}

//******************************REFERRED BELLMAN FORD PSEUDO CODE FROM COURSE SLIDES *************


//Builds Initial Forwarding Table from the information supplied
void build_forwarding_table()
	{

	vforward.clear();
	int mypos=findpos(my_id);

	int i;
	for(i=0;i<vdv[mypos].vrt.size();i++)
		{

		struct forwarding_table f;
		f.dest=vdv[mypos].vrt[i].router_id;
		f.next_hop=vdv[mypos].vrt[i].next_hop;
		f.cost=vdv[mypos].vrt[i].cost;


		vforward.push_back(f);


		}

	}


//Helper function to find next hop of any given router
uint16_t find_next_hop_port(uint32_t next_hop_ip)
{

int i;
for(i=0;i<vcp.size();i++)
{
if(next_hop_ip==vcp[i].router_ip) break;

}

return vcp[i].data_port;


}


//****************** TAKEN FROM BEEJ GUIDE *****************************


//Establish connection to the router to which file has to be transferred
int establish_data_connection(uint32_t next_hop_ip,uint16_t port)

{

next_hop_socket=socket(AF_INET,SOCK_STREAM,0);
if(next_hop_socket<0) cout<<"Cannot create data socket to next hop";

bzero(&connect_next_hop,sizeof(connect_next_hop));
connect_next_hop.sin_family=AF_INET;
connect_next_hop.sin_addr.s_addr=htonl(next_hop_ip);
connect_next_hop.sin_port=htons(port);

int c=connect(next_hop_socket,(struct sockaddr*)&connect_next_hop,sizeof(connect_next_hop));
if(c<0) perror("Problem with connecting to data socket");
else
{

cout<<"Successfully connected to data socket"<<flush;
sockets_connected.push_back(next_hop_socket);

}



return next_hop_socket;

}


//**************** TAKEN FROM BEEJ GUIDE ***************//




uint16_t findhop(uint32_t ip)
{

int i,j;
for(i=0;i<vcp.size();i++)
{

if(vcp[i].router_ip==ip) break;

}

//find the next hop of that id

for(j=0;j<vforward.size();j++)
{

if(vcp[i].router_id==vforward[j].dest) break;

}


return vforward[j].next_hop;

}

//Helper function to receive all the bytes that has being sent
        
	int recvALL(int sock_index, unsigned char *buffer, int nbytes)
{
   int bytes = 0;
    bytes = recv(sock_index, buffer, nbytes, 0);

    if(bytes == 0) return -1;
    while(bytes != nbytes)
        bytes += recv(sock_index, buffer+bytes, nbytes-bytes, 0);

    return bytes;
}


//Helper function to send all the bytes in the buffer
int sendALL(int sock_index, unsigned char *buffer, int nbytes)
{
    int bytes = 0;
    bytes = send(sock_index, buffer, nbytes, 0);

    if(bytes == 0) return -1;
    while(bytes != nbytes)
        bytes += send(sock_index, buffer+bytes, nbytes-bytes, 0);

    return bytes;
}


	//Generates control response to be sent to the controller 
	unsigned char* generate_control_response(uint32_t l,uint8_t code,uint16_t payload_len)
	{
	 unsigned char * b;
	 b=new unsigned char[1024];	
	 b[0]=(l>>24)&(0xFF);
         b[1]=(l>>16)&(0xFF);
         b[2]=(l>>8)&(0xFF);
         b[3]=(l)&0xFF;


        b[4]=code;
        b[5]=0&0xFF;

        b[6]=payload_len>>8&(0xFF);
        b[7]=payload_len&(0xFF);

        return b;

	}

	//Sends Periodic Routing Updates to its neighbors
	void sendupdates( int udp_socket )
	{ 
		unsigned char  buffer[1024];


		memset(buffer,0,sizeof(buffer));
		buffer[0]=nodes>>8 & (0xFF);
		buffer[1]=nodes & (0xFF);
		buffer[2]=my_udp_port>>8 &(0xFF);
		buffer[3]=my_udp_port & (0xFF);

		printf("\nInside send updates ip is %u",my_ip);
		buffer[4]=my_ip >> 24 & (0xFF);
		buffer[5]=my_ip >> 16 & (0xFF);
		buffer[6]=my_ip>>8 & (0xFF);
		buffer[7]=my_ip & (0xFF);
		
		int len=8,s;

			int mypos=findpos(my_id); 

		for(int r=0;r<dv.vrt.size(); ++r)
		{
			buffer[len]=vdv[mypos].vrt[r].router_ip >> 24 & (0xFF);
			buffer[len+1]=vdv[mypos].vrt[r].router_ip >> 16 & (0xFF);
			buffer[len+2]=vdv[mypos].vrt[r].router_ip >> 8 & (0xFF);
			buffer[len+3]=vdv[mypos].vrt[r].router_ip & (0xFF);
			buffer[len+4]=vdv[mypos].vrt[r].router_port >>8 & (0xFF);
			buffer[len+5]=vdv[mypos].vrt[r].router_port & (0xFF);
			buffer[len+6]=0 >>8 & (0xFF);
			buffer[len+7]=0 & (0xFF);
			buffer[len+8]=vdv[mypos].vrt[r].router_id >> 8 & (0xFF);
			buffer[len+9]=vdv[mypos].vrt[r].router_id & (0xFF);
			buffer[len+10]=vdv[mypos].vrt[r].cost >> 8 & (0xFF);
			buffer[len+11]=vdv[mypos].vrt[r].cost & (0xFF);

			len+=12;
		}
	
		
		for(int i=0;i<vneighbor.size();i++)
		{

		struct sockaddr_in dest;
		dest.sin_family=AF_INET;

		int npos=findpos(vneighbor[i].id);
		dest.sin_port=htons(vdv[mypos].vrt[npos].router_port);
		dest.sin_addr.s_addr=htonl(vdv[mypos].vrt[npos].router_ip);
		printf("Udp mesg to be sent to %u " ,dest.sin_addr.s_addr); cout<<flush;
		int s=sendto(udp_socket,buffer,len,0,(struct sockaddr*)&dest,sizeof(dest));

		}
		}


//**************** THE TWO FUNCTIONS BELOW WERE TAKEN FROM BEEJ GUIDE ****************


	//Creates a udp socket to send routing updates to the neighbors
	void create_udp_socket()
	{

	udp_socket=socket(AF_INET,SOCK_DGRAM,0);
	if(udp_socket<0)
        perror("Error in udp socket");


        bzero(&udp_addr,sizeof(udp_addr));
        udp_addr.sin_port=htons(my_udp_port);
        udp_addr.sin_addr.s_addr=htonl(INADDR_ANY);
        udp_addr.sin_family=AF_INET;
        opt=setsockopt(udp_socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
        if(opt==-1)
        {
        perror("Udp setsockopt");
	   }

      b=bind(udp_socket,(struct sockaddr*)&udp_addr,sizeof(udp_addr));
      if(b<0) perror("Bind failed");

     FD_SET(udp_socket,&master_list);
     if(udp_socket>fdmax)
     fdmax=udp_socket;


}
	
	//Create data socket for file transfer

	void create_data_socket()   //listening data socket
	{


		
	memset(&hints,0,sizeof(hints));
	
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;


	char *dataport;
        dataport=(char *)malloc(sizeof(int)+1);
	
	sprintf(dataport, "%u",my_data_port);
	addr=getaddrinfo(NULL,dataport,&hints,&servinfo);
        if(addr!=0)
        {
        fprintf(stderr," %s", gai_strerror(addr));

        }
	
	for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((data_socket = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }


         if (setsockopt(data_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(data_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(data_socket);
            perror("server: bind");
            continue;
        }
        break;
}

        freeaddrinfo(servinfo);
            if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

            if (listen(data_socket,20) == -1) {
        perror("listen");
        exit(1);
    }
else
{

cout<<"Listening for incoming conn on data  port "<<dataport ;

}

	FD_SET(data_socket,&master_list);
	
	if(data_socket>fdmax)
	
	fdmax=data_socket;

	}

//**************** TAKEN FROM BEEJ GUIDE***********************


//************ CONNECTION ESTABLISHMENT,SOCKET CREATION, AND BASIC SELECT LOOP IN THE FUNCTION BELOW REFERENCED  FROM BEEJ GUIDE AND MY  PA1
//***	UTILITY FUNCTIONS 'GETTIMEOFDAY' AND 'TIMERSUB' WERE REFERENCED FROM 
	//http://man7.org/linux/man-pages/man2/gettimeofday.2.html
	//https://linux.die.net/man/3/timersub

	void establish_sockets(unsigned int po)
	{ 
	int initflag=0;
	control_port=po;
	unsigned char *buffer;
	buffer=new unsigned char[1024];	
	int r,i,j;
	
	struct timeval tv;
        socklen_t client_addrlen;

	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);

	char *cport; 
	cport=(char *)malloc(sizeof(int)+1);
	
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	
	sprintf(cport, "%u", control_port);
	addr=getaddrinfo(NULL,cport,&hints,&servinfo);
	if(addr!=0)
	{
	fprintf(stderr," %s", gai_strerror(addr));
	
	}
	
        for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((control_socket = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }


	 if (setsockopt(control_socket, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(control_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(control_socket);
            perror("server: bind");
            continue;
        }
	break;
}

	freeaddrinfo(servinfo);
	    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

	    if (listen(control_socket,20) == -1) {
        perror("listen");
        exit(1);
    }
else
{

cout<<"Listening for incoming conn on control port "<<cport;
}

	//Maintain a list of timers in a list for which updates are to be sent. 

	struct timeval begin,end,diff,timediff;
		
	FD_SET(control_socket,&master_list);
	
	fdmax=control_socket;
	long difference,begin_time,end_time,timer_mil,timer_subtract;
	tv.tv_sec=1.0;
	tv.tv_usec=0.0;
	
	struct timeval min;
	min.tv_sec=1.0;
	min.tv_usec=1.0;

	printf("Initial timer %ld.%06ld " ,tv.tv_sec); cout<<flush;

		while(1)
	
		{
		tv=min;
		printf("\nTImer started for %ld",tv.tv_sec);
		
		bzero(&begin,sizeof(begin));
		bzero(&end,sizeof(end));
		

		 gettimeofday(&begin,NULL);                                              
	

		cout<<flush;
		watch_list=master_list;  
		selret=select(fdmax+1,&watch_list,NULL,NULL,&tv);

		gettimeofday(&end,NULL);
		
		timersub(&end,&begin,&diff) ;    //elapsed time

		
	
		uint16_t last_set_id;
		int l,k;

		for( l=0;l<mul_timer.size();l++)
		{

		//do the min part here
		
		if(l==0)
		{

		min=mul_timer[l].interval;
		last_set_id=mul_timer[l].id;

		}

		timersub(&mul_timer[l].interval,&diff,&timediff);   //subtract
		mul_timer[l].interval=timediff;

		}
		
		
			//printing timer structure

		for(l=0;l<mul_timer.size();l++)
		{

		cout<<"ID "<<"\t" << mul_timer[l].id<<"\t Timer   " << mul_timer[l].interval.tv_sec;

		}


		for(l=0;l<mul_timer.size();l++)
		{

		if(mul_timer[l].interval.tv_sec<min.tv_sec)
		{

		
		min=mul_timer[l].interval;
		last_set_id=mul_timer[l].id;
		}

	
		}

		 if(min.tv_sec<0)
		{
		min.tv_sec=0.1;	
		min.tv_usec=1000;
		

		}
		
		
		printf("After timer updation %ld.%06ld ", tv.tv_sec);
		
			if(selret==-1)
			{	perror("Select failed");  }



			else if(selret==0)   //triggered every 1 sec

                        {
			cout<<"Select 0"<<flush;
			if(initflag==1)
			{	
            		int myidpos,q;

		

			if(last_set_id==my_id)
			{

			 cout<<"\nCalling bellman ford"<<flush;
                        bellman_ford();
			build_forwarding_table();
                        sendupdates(udp_socket);
			
			for(m=0;m<mul_timer.size();m++)
			{

			if(mul_timer[m].id==last_set_id)
			{
			mul_timer[m].interval.tv_sec=(long)interval;
			mul_timer[m].interval.tv_usec=0.0;
			

			}

			}			
			
			}


			
			else 


			{

			for(k=0;k<vneighbor.size();k++)

				{

				if(last_set_id==vneighbor[k].id)    //crash

				{

				cout<<"Neighbor crashed";

				for(z=0;z<mul_timer.size();z++)
				{

				if(mul_timer[z].id==last_set_id) break;

				}

				mul_timer.erase(mul_timer.begin()+z);


				int npos=findpos(vneighbor[k].id);
				int myidpos=findpos(my_id);
				
				vneighbor.erase(vneighbor.begin()+k);
				vdv[myidpos].vrt[npos].next_hop=65535;
				vdv[myidpos].vrt[npos].cost=65535;
				vneighbor[k].cost=65535;


				vlink[npos].cost=65535; vlink[npos].id=65535;
				}
						
				}
          
	}


		for(i=0;i<mul_timer.size();i++)
		{

		if(i==0)
		{
	
		min=mul_timer[i].interval;

		}

		if(min.tv_sec>mul_timer[i].interval.tv_sec)
		min=mul_timer[i].interval;
		
		}

		if(min.tv_sec<0){ min.tv_sec=0.1;  min.tv_usec=1000;   }
			
	
         }// if initflag==1        

	else
	{

	min.tv_sec=1.0;
	
	}

                        }  //end of selret==0  

			else
			{
				
				for(sock_index=0;sock_index<=fdmax;sock_index++)
				{
	
					if(FD_ISSET(sock_index,&watch_list))
					{	
					
						if(sock_index==control_socket)
						{

							
						cout<<"Control SOcket value:" <<sock_index ;
						cout<<flush;
					
					client_addrlen=sizeof(client_addr);
					fdaccept=accept(control_socket,(struct sockaddr*)&client_addr,&client_addrlen);
		
					if(fdaccept<0)
					{perror("Accept failed");}
				else
				{	

					cout<<"Adding new control socket accept "<<fdaccept;
					cout<<flush;
					FD_SET(fdaccept,&master_list);
	
					vcontrol.push_back(fdaccept);    //adding control sockets to list
	
					if(fdaccept>fdmax)
					{
						fdmax=fdaccept;
				

						cout<<"fdmax updated to "<<fdmax;
						}
					
					 getpeername(control_socket,(struct sockaddr*)&client_addr,&client_addrlen);

 
						} }


						else if(sock_index==data_socket)

						{

						cout<<"\nData socket value"<<sock_index;

						cout<<flush;
						
						client_addrlen=sizeof(client_addr);
						fdaccept=accept(data_socket,(struct sockaddr*)&client_addr,&client_addrlen);

						if(fdaccept<0) perror("Data socket accept failed");

						else

						{
						cout<<"Adding new data socket accept"<<fdaccept;
						cout<<flush;
						FD_SET(fdaccept,&master_list);		

						vdata.push_back(fdaccept);
	
						if(fdaccept>fdmax)
						fdmax=fdaccept;

						cout<<"FDMAX updated to"<<fdmax<<flush;
						
						getpeername(data_socket,(struct sockaddr*)&client_addr,&client_addrlen);
							
						}
						}
				
						else if(sock_index==udp_socket)  
						{   int v,mypo,l;

						unsigned char msg[1024];
						memset(msg,0,sizeof(msg));
	
						int len=8+(12*nodes);
						struct sockaddr_in recv_addr; socklen_t recv_addrlen=sizeof(recv_addr);
						int recv=recvfrom(sock_index,msg,len,0,(struct sockaddr*)&recv_addr,&recv_addrlen);
						
						uint16_t fields=msg[0] << 8 | msg[1];
						uint16_t source_port=msg[2] << 8 | msg[3];
						uint32_t source_ip=msg[4] <<24 | msg[5] << 16 | msg[6]<< 8 | msg[7];
						int z=8;
					
						for(i=0;i<vdv.size();i++)
						{
								
							if(vdv[i].router_ip==source_ip) break;    // who sent the update
						}


							printf(" UpdaTe from %u ", vdv[i].id);			
								
						int po=findpos(vdv[i].id);    // position of the router who sent the update in my dv 


					for(l=0;l<mul_timer.size();l++)
					{

					if(mul_timer[l].id==vdv[i].id)   //already exists
					{

					
					mul_timer[l].interval.tv_sec=3*interval;
					break;

					}

					}

	
					if(l==mul_timer.size())
					{
					bzero(&t,sizeof(t));
					t.id=vdv[i].id;
					t.interval.tv_sec=(long)3*interval;
					mul_timer.push_back(t);	
					
					}
					
						mypo=findpos(my_id);

						vdv[mypo].vrt[po].inc_time=0;
						
						for(j=0;j<fields;j++)
						{
						
						vdv[po].vrt[j].router_ip=msg[z]<<24 | msg[z+1]<<16 | msg[z+2] <<8 | msg[z+3]; z+=4;
						vdv[po].vrt[j].router_port= msg[z]<<8 | msg[z+1]; z+=2;
						uint16_t pad=msg[z] <<8 | msg[z+1] ; z+=2;
						vdv[po].vrt[j].router_id= msg[z] << 8 | msg[z+1]; z+=2;
						vdv[po].vrt[j].cost=msg[z]<< 8 | msg[z+1]; z+=2;
										
}
		
						}       
						else

						{

						if(find(vcontrol.begin(),vcontrol.end(),sock_index)!=vcontrol.end())
						{
						cout<<"\nReady to Read";
						cout<<flush;
						
						memset(buffer,0,1024);
						r=recv(sock_index,buffer,8,0);
						if(r<=0)
						{
						

						if(r==0){ cout<<"Server closed";}
						else if(r==-1) {cout<<" Error in receiving";}
						
						FD_CLR(sock_index,&master_list);						
						close(sock_index);
						vcontrol.erase(std::remove(vcontrol.begin(),vcontrol.end(),sock_index),vcontrol.end());    //removing the control socket from my vector					
						}
		
						
						else if(r>0)
						{
						
						cout<<"\nGot message of "<<r<<"bytes";
						cout<<flush;
						
						cm=(struct controlmessage*)malloc(sizeof(controlmessage));
						
						
						cm->ip= buffer[0]<<24 | buffer[1]<<16 | buffer[2]<<8 | buffer[3];
						cm->con_code=buffer[4];
						cm->response_time=buffer[5];
						cm->payload_length=buffer[6]<<8 | buffer[7];	
						

						 uint32_t l=ntohl(client_addr.sin_addr.s_addr);  //Controller_IP

							switch(cm->con_code)
							{
						        uint16_t plen;
							int s;
							case 0:	  //Author
						        
							memset(buffer,0,sizeof(buffer));
							 
								 

							 unsigned char *payload;
							 n=sizeof("I, naveenmu, have read and understood the course academic integrity policy.")-1;
							buffer=generate_control_response(l,cm->con_code,n);

							payload=new unsigned char[100];
							strcpy((char*)payload,"I, naveenmu, have read and understood the course academic integrity policy.");
								
								
							s=sendALL(fdaccept,buffer,8);
							s=sendALL(fdaccept,payload,75);
							
							break;
							
	
							case 1:  //Init Message from Controller
							initflag=1;	
							memset(buffer,0,sizeof(buffer));
							r=recvALL(fdaccept,buffer,cm->payload_length);
							cout<<"Payload recvd"<<cm->payload_length<<flush;
	
							nodes=buffer[0]<<8 | buffer[1]; 
							interval=buffer[2]<<8 | buffer[3];
							plen=4;
							

							for(i=0;i<nodes; i++)
							{
							bzero(&cp,sizeof(cp));
							cp.router_id=buffer[plen]<<8 | buffer[plen+1];  
							 plen+=2;

							cp.router_port=buffer[plen]<<8 | buffer[plen+1];
							 plen+=2;

							cp.data_port= buffer[plen] << 8 | buffer[plen+1];
							 plen+=2;
							
							cp.cost= buffer[plen] << 8 | buffer[plen+1]; 
							plen+=2; 
		                                        
							cp.router_ip=buffer[plen]<<24 | buffer[plen+1] <<16 | buffer[plen+2] << 8 | buffer[plen+3] ;
							 plen+=4;
							
							vcp.push_back(cp);

							
							}

							cout<<"\nAfter loading to vcp vector"<<flush;
							for(i=0;i<vcp.size();++i)
							{

								cout<<"\nRouter id      "<<vcp[i].router_id <<"ROuter Port    " <<vcp[i].router_port  << "Data port    " << vcp[i].data_port << "cost   " <<vcp[i].cost;  
							}

							
							for(vector<struct controlpayload>:: iterator m=vcp.begin(); m!=vcp.end(); ++m)

							{
							  bzero(&dv,sizeof(dv));
							dv.id=m->router_id;
							dv.router_ip=m->router_ip;
									
						
						
							if(m->cost==0){
							 my_id = m->router_id;     // myself
							my_udp_port=m->router_port;
							my_data_port=m->data_port;
							my_ip=m->router_ip;
							}
							
							 for(vector<struct controlpayload>:: iterator n=vcp.begin(); n!=vcp.end(); ++n)
							{	  bzero(&rt,sizeof(rt));
								rt.router_ip=n->router_ip;
								rt.router_id=n->router_id;
								rt.next_hop=65535;
								rt.neighbor_status=0;
								rt.cost=65535;
								rt.router_port=n->router_port;
								rt.inc_time=0;

									if(m->router_id==my_id&& n->cost>0 && n->cost < 65535)  //neighbor
									{       struct neighbor ne;
										ne.id=n->router_id;
										ne.cost=n->cost;
										vneighbor.push_back(ne);
											
										 
										rt.next_hop=n->router_id;
										rt.cost=n->cost;
										rt.inc_time=0;
									}


									else if( m->router_id==my_id && n->cost==0)
									{
										
									        rt.next_hop=my_id;
										rt.cost=0;
										rt.inc_time=0;
									}

									else if( m->router_id == n->router_id && m->router_id!=my_id)

									{
											
											rt.cost=0;

											rt.inc_time=0;
									}
	
								
								dv.vrt.push_back(rt);																	
								if(m->router_id==my_id)
								{
								bzero(&li,sizeof(li));
                                                        	li.id=rt.next_hop;
                                                      	  	li.cost=rt.cost;
                                                        	vlink.push_back(li);
							}
									
							
							}
							
							
							vdv.push_back(dv);      //adding to vector dvbyid containing id and respective distance vector	
									
					}
			
							memset(buffer,0,sizeof(buffer));
							 n=0;
							buffer=generate_control_response(l,cm->con_code,n);
							
							 s=sendALL(fdaccept,buffer,8);
						

							//--adding my interval to set of timers
								
							bzero(&t,sizeof(t));
							t.id=my_id;
							t.interval.tv_sec= (long)interval;
							t.interval.tv_usec=0.0;
							mul_timer.push_back(t);
							
							//--
		
							//establish udp and data socket
					
        						 create_udp_socket();
							 create_data_socket();  
 
							break;

						
							case 2:  // Routing Table

							memset(buffer,0,sizeof(buffer));
						
							n=8*nodes;
							printf("Total bytes to be sent is %u",n);
						        buffer=generate_control_response(l,cm->con_code,n);	
							
							
							s=sendALL(sock_index,buffer,8);
							memset(buffer,0,sizeof(buffer));
						
							
								
							for(i=0;i<vdv.size();++i)
							{
							  if(vdv[i].id==my_id)
								{break;}

							}
								plen=0;
							for(j=0;j<dv.vrt.size(); ++j)
							{
								
								buffer[plen]=vdv[i].vrt[j].router_id>> 8 & (0xFF);
								buffer[plen+1]=vdv[i].vrt[j].router_id & (0xFF);
								buffer[plen+2]=0 >>8 & (0xFF);
								buffer[plen+3]=0 & (0xFF);
								buffer[plen+4]= vdv[i].vrt[j].next_hop >>8 & (0xFF);
								buffer[plen+5]=vdv[i].vrt[j].next_hop & (0xFF);
								buffer[plen+6]= vdv[i].vrt[j].cost >>8 & (0xFF);
								buffer[plen+7]=vdv[i].vrt[j].cost & (0xFF);
								
								plen+=8;
							}             
					
							s=sendALL(fdaccept,buffer,plen);
				
							break;
							
						case 3:   // Update 

						memset(buffer,0,sizeof(buffer));
						s=recvALL(sock_index,buffer,4);
						
						
						 uid=buffer[0] << 8 | buffer[1];
						 ucost=buffer[2] << 8 | buffer[3];
						for(i=0;i<vneighbor.size();++i)
						{
							
						if(uid==vneighbor[i].id) break;
						
						}
						vneighbor[i].cost=ucost;

					 for(j=0;j<vlink.size();j++)
                                                {

                                                if(uid==vlink[j].id) break;

                                                }


                                                vlink[j].cost=ucost;
						memset(buffer,0,sizeof(buffer));

						buffer=generate_control_response(l,cm->con_code,0);
						s=sendALL(sock_index,buffer,8);

						
						break;

						case 4:  //Crash

						memset(buffer,0,sizeof(buffer));
						buffer=generate_control_response(l,cm->con_code,0);
						s=sendALL(sock_index,buffer,8);
						FD_CLR(sock_index,&master_list);
						exit(0);
						break;



						case 5:  //sendfile
			
						memset(buffer,0,sizeof(buffer));
						
						r=recvALL(sock_index,buffer,cm->payload_length);

						sf.dest_ip=buffer[0]<<24 | buffer[1] <<16 | buffer[2]<<8 | buffer[3];
						sf.ttl=buffer[4];
						sf.tid=buffer[5];		
						sf.seq_num=buffer[6]<< 8 | buffer[7] ;  // source_seqnum=sf.seq_num-1;
						z=0;
						for(i=8;i<cm->payload_length;i++)
						{
						
						sf.filename[z]=buffer[i];
						z++;		
						}
					
						
	
						cout<<"FIlename is "<<sf.filename;

							
						hop=findhop(sf.dest_ip);			
						
						cout<<"\n FIle is to be routed to " <<(unsigned int)hop<<flush;
				
						for(i=0;i<vcp.size();i++)
						{

				
						if(vcp[i].router_id==hop) 
						{ next_hop_ip=vcp[i].router_ip;    //ip of the next hop
	
							break;
							
						}
	
						}

						cout<<"\nThe ip of next hop is "<<(unsigned int)next_hop_ip;

						
						
						
						next_hop_port=find_next_hop_port(next_hop_ip);


						
						data_sock1=establish_data_connection(next_hop_ip,next_hop_port);   //create connection to the first hop

						FD_SET(data_sock1,&master_list);

						if(data_sock1>fdmax) fdmax=data_sock1;
						vdata.push_back(data_sock1);							
					
						x=0;

						fp=fopen(sf.filename,"rb");
						if(fp==NULL) { printf("Error in reading file");exit(1);}


//**************** FSEEK REFERENCED FROM http://man7.org/linux/man-pages/man3/fseek.3.html*****************




                                                fseek(fp,0,SEEK_END);
                                                 lsize=ftell(fp);
                                                rewind(fp);
                                                cout<<"\nFIle size= " <<lsize;

                                                fragments=lsize/1024;    
						cout<<"\nTotal fragments is"<<fragments<<flush;
 						bytesread=0,fragments_read=0;	
						seqnum=0;	


						memset(sendbuffer,0,sizeof(sendbuffer));
                                                sendbuffer[0]=sf.dest_ip>>24 & (0xFF);
                                                sendbuffer[1]=sf.dest_ip>>16 & (0xFF);
                                                sendbuffer[2]=sf.dest_ip >>8 & (0xFF);
                                                sendbuffer[3]=sf.dest_ip & (0xFF);
                                                sendbuffer[4]=sf.tid &(0xFF);
                                                sendbuffer[5]=sf.ttl & (0xFF);
						sendbuffer[8]=0 & (0xFF);
                                                sendbuffer[9]=0 & (0xFF) ;
                                                sendbuffer[10]=0 & (0xFF) ;
                                                sendbuffer[11]=0 & (0xFF) ;
						x=0;
						for(i=1;i<=fragments;i++)
						{ plen=12;
						bytesread=fread(file_data,1,1024,fp);
						
						if(bytesread>0 && i<=fragments-1)
						{
						
						sendbuffer[6]=(sf.seq_num+x)>> 8 & (0xFF);
                                                sendbuffer[7]=(sf.seq_num+x) & (0xFF);
						x=x+1;
	

						memcpy(&sendbuffer[12],file_data,1024);
						
						ds=sendALL(data_sock1,sendbuffer,1036);

						if(i==fragments-1)
						memcpy(penultimate,sendbuffer,1036);
						}
									
						else if(bytesread>0 && i==fragments)
						{
					
						
					plen=12;
						 sendbuffer[6]=(sf.seq_num+x)>> 8 & (0xFF);
                                                sendbuffer[7]=(sf.seq_num+x) & (0xFF);

						sendbuffer[8]=padfin>>24 & (0xFF);
                                                sendbuffer[9]=padfin>>16 & (0xFF) ;
                                                sendbuffer[10]=padfin>>8 & (0xFF) ;
                                                sendbuffer[11]=padfin & (0xFF) ;

			
					memcpy(&sendbuffer[12],file_data,1024);
					ds=sendALL(data_sock1,sendbuffer,1036);
					memcpy(last,sendbuffer,1036);
					memset(buffer,0,sizeof(buffer));

                                     buffer=generate_control_response(l,cm->con_code,0);
                                      s=sendALL(sock_index,buffer,8);

					shutdown(data_sock1,SHUT_WR);
					
				fclose(fp);	
					}


					}

					
					
						bzero(&st,sizeof(st));

						st.tid=sf.tid;
						st.ttl=sf.ttl;	
						 
						for(sq=sf.seq_num;sq<=(sf.seq_num+x);sq++)

						{
	
						st.seq.push_back(sq);
							
						}	
		
						vsend_stats.push_back(st);

														
						

						//last data packet
								
						      
						
						break;
	
							
						case 6: //sendfile stats
					



						memset(buffer,0,sizeof(buffer));
						s=recvALL(sock_index,buffer,1);
						stats_tid=buffer[0];
				
						
						for(i=0;i<vsend_stats.size();i++)
						{

						if(vsend_stats[i].tid==stats_tid) break;


						} 
						
						memset(buffer1,0,sizeof(buffer1));

						buffer1[0]=vsend_stats[i].tid;
						buffer1[1]=vsend_stats[i].ttl;
						buffer1[2]=0 >> 8 & (0xFF);
						buffer1[3]=0 & (0xFF);
						
						
					        //cout<<"vsend stats.tid is"<<(unsigned int)vsend_stats[i].tid<<"Size of vectors is"<<vsend_stats[i].seq.size()<<flush;
						plen=0;
						memset(buffer,0,sizeof(buffer));
						
							
							
					        buffer=generate_control_response(l,cm->con_code,(4+(vsend_stats[i].seq.size() * 2)));
						s=sendALL(sock_index,buffer,8);
						s=sendALL(sock_index,buffer1,4);
						
						memset(buffer,0,sizeof(buffer));

						for(j=0;j<vsend_stats[i].seq.size();j++)
						{
				
						buffer[plen]=vsend_stats[i].seq[j]>>8 & (0xFF);
						buffer[plen+1]=vsend_stats[i].seq[j] & (0xFF);
						plen+=2;
    
						count+=2;
						if(count %1024==0  | (count==vsend_stats[i].seq.size()*2)   )
			
					{
						s=sendALL(sock_index,buffer,plen); 		
						memset(buffer,0,sizeof(buffer)); 
						plen=0;
                          	               
				
			 		 }		
							
				
						}    

							
					break;


					case 7:
					//last packet
					memset(buffer,0,sizeof(buffer));
					buffer=generate_control_response(l,cm->con_code,1036);
					s=sendALL(sock_index,buffer,8);
					s=sendALL(sock_index,last,1036);
					break;

					case 8:  //penultimate
					memset(buffer,0,sizeof(buffer));
					buffer=generate_control_response(l,cm->con_code,1036);
					s=sendALL(sock_index,buffer,8);
					s=sendALL(sock_index,penultimate,1036);
						break;

					
							}  // end of switch control code
							
						
						}   // end of r>=1


						}  //end of if socket is a control socket



					else if(find(vdata.begin(),vdata.end(),sock_index)!=vdata.end())      // if data socket is there in that list
					{

					
					cout<<"\nGot a new request for sending data";	
					      
										
					cout<<"Inside recv"<<flush;


					memset(sendbuffer,0,sizeof(sendbuffer));
					memset(data_payload,0,sizeof(data_payload));
					bytesread=0;
					bytesread=recvALL(sock_index,sendbuffer,1036);	
				        if(bytesread<=0)
					{
						shutdown(sock_index,SHUT_RD);

						FD_CLR(sock_index,&master_list);
                              
                                         vdata.erase(std::remove(vdata.begin(),vdata.end(),sock_index),vdata.end());

					if(fpwrite!=NULL)  fclose(fpwrite);
			
			
					}                                    
                                        

					else
					{
					
					bzero(&dp,sizeof(dp));
                                        dp.dest_ip=sendbuffer[0]<<24 | sendbuffer[1] <<16 | sendbuffer[2] <<8 | sendbuffer[3];

	   				cout<<"Dest ip is"<<(unsigned int)dp.dest_ip<<flush;

                                        dp.tid=sendbuffer[4];

                                        dp.ttl=sendbuffer[5];

                                        dp.seq_num=sendbuffer[6] << 8 |sendbuffer[7];

                                        dp.padding=sendbuffer[8] <<24 | sendbuffer[9]<<16 | sendbuffer[10] << 8 | sendbuffer[11];

					cout<<"Fin bit is"<<(unsigned int)dp.padding<<flush;
				  	cout<<"Seq num is"<<(unsigned int)dp.seq_num<<flush;
					
					int pl=0;
					for(i=12;i<1036;i++)
					{

					
					data_payload[pl]=sendbuffer[i];	
					pl+=1;

					}
							
					dp.ttl=dp.ttl-1;
					sendbuffer[5]=dp.ttl & (0xFF);
					if(dp.ttl>0)

					{
					present=check_stats(dp.tid);
					if(present==-1)
					{

					bzero(&st,sizeof(st));
					st.tid=dp.tid;
					st.ttl=dp.ttl;
					st.seq.push_back(dp.seq_num);
					vsend_stats.push_back(st);				
				

					}
					
					else if(present>-1 && present< vsend_stats.size()) 
					{

					vsend_stats[present].seq.push_back(dp.seq_num);

				        }
					 			
					if(last[0]=='\0')
					{


					memcpy(last,sendbuffer,1036);
					}

					else
					{

			memcpy(penultimate,last,1036);
			memcpy(last,sendbuffer,1036);

					}	
					if(fpwrite==NULL)
					{
					memset(filestore,0,sizeof(filestore));
					strcpy(filestore,"file-");
                                        sprintf(tidstore,"%u",dp.tid);
                                        strcat(filestore,tidstore);
                                        cout<<"\nName of the file is"<<filestore<<flush;
   					}
					if(dp.dest_ip==my_ip)    //packet for me
					{
					
					if(fpwrite==NULL)
					{
					cout<<"Seq num "<<dp.seq_num;
					fpwrite=fopen(filestore,"ab");
					fwrite(data_payload,1,1024,fpwrite);
				
					}
		
					else
					{

					fwrite(data_payload,1,1024,fpwrite);

					}
										
									
			         	}   // dest_ip==my_ip


					else

										   //route the packet
					{


				if(find(existing.begin(),existing.end(),dp.tid)!=existing.end())
				{

                                  ds=sendALL(data_sock2,sendbuffer,1036);

				}



					else
					{


	                                hop=findhop(dp.dest_ip);

                                        cout<<"\nFIle is to be routed to"<<hop<<flush;
                                        for(i=0;i<vcp.size();i++)
                                        {


                                        if(vcp[i].router_id==hop)
                                       {
                                         next_hop_ip=vcp[i].router_ip;    //ip of the next hop

                                              break;

                                       }

                                       }

					cout<<"\nThe ip of next hop is "<<(unsigned int)next_hop_ip;


				 	next_hop_port=find_next_hop_port(next_hop_ip);



                       		       data_sock2=establish_data_connection(next_hop_ip,next_hop_port);   //create connection to the first hop
                                      FD_SET(data_sock2,&master_list);

                                    	  if(data_sock2>fdmax) fdmax=data_sock2;
                          		vdata.push_back(data_sock2);
                          		existing.push_back(dp.tid);
					ds=sendALL(data_sock2,sendbuffer,1036);

						}


				     if(dp.padding ==2147483648)
				    {    cout<<"Inside dp padding"<<flush;
				      
				      shutdown(data_sock2,SHUT_WR);

				 

			}
						
				  } //route the packet
					
					}  //ttl>0

					
					else if(dp.ttl<=0)
					{

					cout<<"\nPacket dropped"<<flush;
					
					}
					
					
					}   //if bytesread>0 				
			
							
					}// end of existing data conn


						} // end of else (read from exisiting connections

						

			

					}  // end of fd_isset


				} // end of for	
			}	// if selret >0
	
		
				
		
		}//end of while(1)	
		
		
	}  // end of function establishsocket

}; // class ends
int main(int argc, char **argv)
{

	
	
	cout<<flush;
	DV dv=*new DV();
	dv.establish_sockets(atoi(argv[1]));
	return 0;
}
