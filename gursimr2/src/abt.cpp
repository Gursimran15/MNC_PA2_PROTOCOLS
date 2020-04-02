#include "../include/simulator.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <bits/stdc++.h> 
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

pkt packtob,packfroma,acktoa,ackfromb;

int bufferseqnum,c,recv;
map<int, string> buffer;
int changeseqno(int s){
  if(s==1){
          return 0;
        }
        else{
          return 1;
        }

}
int changeackno(int a){
  if(a==1){
          return 0;
        }
        else{
          return 1;
        }

}
void makepacket(pkt &p,int n){
p.checksum=0;
for(int i=0;i<20;i++){
  p.payload[i]=buffer[n][i];
  p.checksum+=(int)p.payload[i];
}
p.checksum += p.seqnum + p.acknum;
}
/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
  printf("I am here 1\n"); //for debug
  buffer[bufferseqnum]=message.data;
  bufferseqnum++;
  printf("I am here 2\n"); //for debug
            if(recv){
              
              printf("I am here 3\n"); //for debug
                    makepacket(packtob,c);
                    tolayer3(0,packtob);
                    starttimer(0,15);
                    recv=0;            
                  }
                  printf("I am here 4\n"); //for debug
 }


/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  int checksum=0; 
  checksum+= packet.seqnum + packet.acknum;
  printf("I am here 7\n"); //for debug
printf("%d %d\n",packtob.seqnum,packet.seqnum); //for debug
printf("%d %d\n",packtob.acknum,packet.acknum); //for debug
printf("%d %d\n",checksum,packet.checksum); //for debug
  if(checksum == packet.checksum && packet.acknum==packtob.acknum){ // If not corrupted and has expected ack number
    stoptimer(0);
    packtob.seqnum=changeseqno(packtob.seqnum);
    packtob.acknum=changeackno(packtob.acknum);
    printf("%d %d\n",packtob.seqnum,packet.seqnum); //for debug
    printf("%d %d\n",packtob.acknum,packet.acknum); //for debug
    c++;
    recv=1;
  }
  else{
    //donothing
  }
}
/* called when A's timer goes off */
void A_timerinterrupt()
{
 starttimer(0,30);
 tolayer3(0,packtob);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  c=1;
  recv=1;
packtob.seqnum = 0;
packtob.acknum = 0;
packtob.checksum = 0;
bufferseqnum=1;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  printf("I am here 5\n");//for debug
  int checksum = 0;
for(int i=0;i<20;i++){
  checksum += (int)packet.payload[i];
}
printf("I am here 6\n"); //for debug
checksum += packet.seqnum + packet.acknum;
printf("%d %d\n",checksum,packet.checksum); //for debug
printf("I am here 6\n"); //for debug
printf("%d %d\n",packfroma.seqnum,packet.seqnum); //for debug
if(checksum == packet.checksum && packet.seqnum==packfroma.seqnum){// If not corrupted and has expected sequence number
  tolayer5(1,packet.payload);
packfroma.checksum=packfroma.seqnum+packfroma.acknum;
tolayer3(1,packfroma);
packfroma.seqnum=changeseqno(packfroma.seqnum);
packfroma.acknum=changeackno(packfroma.acknum);
}
else{
  tolayer3(1,packfroma);
}
 }

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
//  count=0;
packfroma.seqnum = 0;
packfroma.acknum = 0;
packfroma.checksum=0;
}
