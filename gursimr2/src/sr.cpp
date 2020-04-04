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

void A_output(struct msg message)
{
  strncpy(buffer[bufferseqnum-1].payload,message.data,sizeof(message.data));
  buffer[bufferseqnum-1].seqnum = bufferseqnum;
  buffer[bufferseqnum-1].acknum = bufferseqnum;
  buffer[bufferseqnum-1].checksum = 2*bufferseqnum;
  for(int i=0;i<20;i++)
  buffer[bufferseqnum-1].checksum+=(int)buffer[bufferseqnum-1].payload[i];
  bufferseqnum++;

  if(nextseqnum >= sendbase && nextseqnum < sendbase + window){
    
    wait.push(nextseqnum);
    rs[nextseqnum-1]=0;
    st[nextseqnum-1]=get_sim_time();
    tolayer3(0,buffer[nextseqnum-1]);

    cout<<buffer[nextseqnum-1].payload;
    
    if(nextseqnum==1){
      starttimer(0,15);
    }
    nextseqnum++;
  }
  
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
   int checksum = packet.seqnum + packet.acknum;
if(packet.checksum == checksum){

 if(packet.acknum >= sendbase && packet.acknum < sendbase + window){
   
   rs[packet.seqnum - 1]=1;
   
   if(sendbase==packet.acknum){
     while(rs[sendbase-1]==1)
     sendbase++;
   }
   
   stoptimer(0);
   while(rs[wait.front()-1]==1)
   wait.pop();
   starttimer(0,15.0 + st[wait.front()-1] - get_sim_time());

 }

}

}

/* called when A's timer goes off */
void A_timerinterrupt()
{
int p = wait.front();
wait.pop();
wait.push(p);
tolayer3(0,buffer[p-1]);

while(rs[wait.front()-1]==1)
wait.pop();
st[p-1]=get_sim_time();
starttimer(0,15.0 + st[wait.front()-1] - st[p-1]);
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
  cout<<buffer[nextseqnum-1].payload;

   int checksum = packet.seqnum + packet.acknum;
  for(int i=0;i<20;i++)
  checksum += (int)packet.payload[i];
if(packet.checksum == checksum){
  ack.seqnum = packet.seqnum;
  ack.acknum = packet.acknum;
  if(packet.seqnum>=rbase && packet.seqnum < rbase + window){
    tolayer3(1,ack);
    
    if(rr[packet.seqnum-1]!=1){
    rr[packet.seqnum-1]=1;
    rbuffer[packet.seqnum-1]=packet;
    }

    if(packet.seqnum == rbase){
      tolayer5(1,packet.payload);
      rbase++;
      while(rr[packet.seqnum-1]==1){
        tolayer5(1,rbuffer[packet.seqnum-1].payload);
        rbase++;
      }
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
