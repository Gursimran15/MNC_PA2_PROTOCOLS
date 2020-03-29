#include "../include/simulator.h"
#include <string.h>
#include <stdio.h>
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
using namespace std;
/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
pkt packtob,packfroma,acktoa,ackfromb;
char buffer[1500];
int count;
/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
  count++;
 strncat(buffer,message.data,sizeof(message.data)); 
 packtob.checksum=0;
 for(int i=0;i<20;i++){
   packtob.payload[i]=buffer[(count-1)*20 + i];
   packtob.checksum += (int)packtob.payload[i];
 }
 packtob.checksum += packtob.seqnum + packtob.acknum;
  printf("%d \n",packtob.checksum);
  printf("%d \n",packtob.seqnum);
  printf("%d \n",packtob.acknum);
 tolayer3(0,packtob);
 starttimer(0,15);
  printf("%s","Message sent and timer started\n");
 if(packtob.acknum==1){
   packtob.acknum=0;
 }
 else{
   packtob.acknum=1;
 }
 printf("%d \n",packtob.checksum);
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  ackfromb.checksum = packet.seqnum + packet.acknum;
   if(packet.acknum != packtob.acknum){
   }
  if(packet.checksum != ackfromb.checksum){
  }
  if(packet.checksum == ackfromb.checksum){ //if not corrupted
    if(packet.acknum == packtob.acknum){
     stoptimer(0);
     printf("%s","Acknowledgement received\n");
     if(packtob.seqnum==1){
          packtob.seqnum=0;
        }
        else{
          packtob.seqnum=1;
        }
            
    }
  }
}
/* called when A's timer goes off */
void A_timerinterrupt()
{
tolayer3(0,packtob);
 starttimer(0,15);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  count=0;
packtob.seqnum = 0;
packtob.acknum = 0;
packtob.checksum = 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
 packfroma.checksum=0;
 for(int i=0;i<20;i++){
   packfroma.payload[i]=packet.payload[i];
   packfroma.checksum += (int)packfroma.payload[i];
 } 
 printf("%s","before checksum\n");
 packfroma.checksum += packfroma.seqnum + packfroma.acknum;
 printf("%s","after checksum\n");
 if(packet.seqnum != packfroma.seqnum){ // for duplicate
   if(packet.seqnum == 0){
     acktoa.seqnum = packfroma.seqnum;
     acktoa.acknum = 1;
     acktoa.checksum = acktoa.seqnum + acktoa.acknum;
     tolayer3(1,acktoa);
   }else{
     acktoa.seqnum = packfroma.seqnum;
     acktoa.acknum = 0;
     acktoa.checksum = acktoa.seqnum + acktoa.acknum;
     tolayer3(1,acktoa);
   }
 }
 if(packet.checksum != packfroma.checksum){ // If corrupted
   if(packet.seqnum == 0){
     acktoa.seqnum = packfroma.seqnum;
     acktoa.acknum = 0;
     acktoa.checksum = acktoa.seqnum + acktoa.acknum;
     tolayer3(1,acktoa);
   }else{
     acktoa.seqnum = packfroma.seqnum;
     acktoa.acknum = 1;
     acktoa.checksum = acktoa.seqnum + acktoa.acknum;
     tolayer3(1,acktoa);
   }

 }
 printf("%s","before correct checksum\n");
 printf("%d %d\n",packet.checksum,packfroma.checksum);
 printf("%d %d\n",packet.seqnum,packfroma.seqnum);
  printf("%d %d\n",packet.acknum,packfroma.acknum);
 if(packet.checksum == packfroma.checksum){ 
   printf("%s","correct checksum\n");
   //if not corrupted
   if(packet.seqnum == 0){
     acktoa.seqnum = packfroma.seqnum;
     acktoa.acknum = 1;
     acktoa.checksum = acktoa.seqnum + acktoa.acknum;
     tolayer3(1,acktoa);
    tolayer5(1,packfroma.payload);
   }else{
     acktoa.seqnum = packfroma.seqnum;
     acktoa.acknum = 0;
     acktoa.checksum = acktoa.seqnum + acktoa.acknum;
     tolayer3(1,acktoa);
     tolayer5(1,packfroma.payload);
   }
   printf("I am here");
        if(packfroma.seqnum==1){
      packfroma.seqnum=0;
    }
    else{
      packfroma.seqnum=1;
    }
     if(packfroma.acknum==1){
      packfroma.acknum=0;
    }
    else{
      packfroma.acknum=1;
    }
    printf("%d \n",packfroma.seqnum);
  printf("%d \n",packfroma.acknum);
 }

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
 count=0;
packfroma.seqnum = 0;
packfroma.acknum = 0;
packfroma.checksum=0;
}
