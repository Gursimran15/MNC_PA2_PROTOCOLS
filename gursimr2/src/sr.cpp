#include "../include/simulator.h"
#include <bits/stdc++.h>
#include <queue> 
using namespace std;
/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
int sendbase,nextseqnum,rbase,window,bufferseqnum;
queue <int> wait;
pkt buffer[1000];
pkt rbuffer[1000];
pkt ack;
int rs[1000],rr[1000];
float st[1000];
char rcv[20];

void A_output(struct msg message)
{
  for(int i=0;i<20;i++)
  buffer[bufferseqnum-1].payload[i]=message.data[i];
  buffer[bufferseqnum-1].seqnum = bufferseqnum;
  buffer[bufferseqnum-1].acknum = bufferseqnum;
  buffer[bufferseqnum-1].checksum = 2*bufferseqnum;
  for(int i=0;i<20;i++)
  buffer[bufferseqnum-1].checksum+=(int)buffer[bufferseqnum-1].payload[i];
  bufferseqnum++;

  if(nextseqnum < sendbase + window){
    
    wait.push(nextseqnum);
    rs[nextseqnum-1]=0;
    st[nextseqnum-1]=get_sim_time();
    tolayer3(0,buffer[nextseqnum-1]);

    cout<<buffer[nextseqnum-1].payload<<"\n";
    cout<<buffer[nextseqnum-1].seqnum<<"\n";
    cout<<buffer[nextseqnum-1].acknum<<"\n";
    cout<<buffer[nextseqnum-1].checksum<<"\n";
    cout<<"\n"<<wait.size();
    if(nextseqnum==sendbase){
      starttimer(0,20.0);
    }
 
    nextseqnum++;
  }
  
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
 cout<<"I am here\n";
    cout<<get_sim_time()<<"\n"<<wait.front();
  cout<<packet.seqnum;
 
   int checksum = packet.seqnum + packet.acknum;
if(packet.checksum == checksum){

 if(packet.acknum >= sendbase && packet.acknum < sendbase + window){
    
    cout<<"I am here\n";
   rs[packet.seqnum - 1]=1;
   if(sendbase==packet.acknum){
     while(rs[sendbase-1]==1){
     if(wait.front()==sendbase)
     wait.pop();
     sendbase++;
     }
  
   }

   while(nextseqnum < sendbase + window && nextseqnum < bufferseqnum){
    
    wait.push(nextseqnum);
    rs[nextseqnum-1]=0;
    st[nextseqnum-1]=get_sim_time();
    tolayer3(0,buffer[nextseqnum-1]);
    nextseqnum++;
  }
  if(sendbase==nextseqnum)
    stoptimer(0); //until i get another packet from above
    cout<<checksum<<" "<<packet.checksum<<"\n";
  //  if(!wait.empty())
  
   cout<<"I am here\n";
    cout<<get_sim_time()<<"\n"<<wait.front();
   
 }

}


}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  if(rs[wait.front()-1]==0){
    tolayer3(0,buffer[wait.front()-1]);
    st[wait.front()-1]=get_sim_time();
  }
int p = wait.front();
wait.pop();
wait.push(p);
float time = get_sim_time() - st[wait.front()-1]; //Getting Relative Time
float ftime = 20.0 - time;
	starttimer(0,ftime);
  }


/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
bufferseqnum = 1;
nextseqnum = 1;
sendbase = 1;
window=getwinsize();
  }

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  cout<<packet.payload<<"\n";
    cout<<packet.seqnum<<"\n";
    cout<<packet.acknum<<"\n";
    cout<<packet.checksum<<"\n";
   int checksum = packet.seqnum + packet.acknum;
  for(int i=0;i<20;i++)
  { checksum += (int)packet.payload[i];
  }
if(packet.checksum == checksum){
  ack.seqnum = packet.seqnum;
  ack.acknum = packet.acknum;
  ack.checksum = packet.seqnum + packet.acknum;
  if(packet.seqnum>=rbase && packet.seqnum < rbase + window){
    tolayer3(1,ack);
    
    if(rr[packet.seqnum-1]!=1){
    rr[packet.seqnum-1]=1;
    rbuffer[packet.seqnum-1].seqnum=packet.seqnum;
    rbuffer[packet.seqnum-1].acknum=packet.acknum;
    rbuffer[packet.seqnum-1].checksum=packet.checksum;
    strncpy(rbuffer[packet.seqnum-1].payload,packet.payload,sizeof(packet.payload));
    }
    
    if(packet.seqnum == rbase){
      do{
        for(int i=0;i<20;i++)
        {rcv[i]=rbuffer[rbase-1].payload[i];
         cout<<rcv[i];
        }
        cout<<"\n";
        tolayer5(1,rcv);
        rbase++;

      }while(rr[rbase-1]==1);
        
      
    }

  }

else{
  if(packet.seqnum>=rbase-window && packet.seqnum < rbase){
   tolayer3(1,ack);
  }
  else{

  }
}

}
cout<<"I am here\n";

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  for(int i=0;i<1000;i++)
  rr[i]=0;
  rbase=1;
  window=getwinsize();
  
}
