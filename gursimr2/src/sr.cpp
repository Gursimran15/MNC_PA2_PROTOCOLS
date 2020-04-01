#include "../include/simulator.h"
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

int base,nextseqnum,winsize,bufferseqnum,rbase,rwindow;
map<int, string> buffer;
map<int,pkt> rbuffer;
map<int,int> r;
map<int,int> rlist;
int rl;
char rcv[20];
pkt sendpkt,bufferpkt,ackpkt; 
void makepacket(pkt &p,int n){
p.checksum=0;
for(int i=0;i<20;i++){
  p.payload[i]=buffer[n][i];
  p.checksum+=(int)p.payload[i];
}
p.seqnum = n;
p.acknum = n;
p.checksum += p.seqnum + p.acknum;
}
void sendpacket(pkt &p){
tolayer3(0,p);
}
/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
  buffer[bufferseqnum]=message.data;
  bufferseqnum++;
   if((nextseqnum) < (base + winsize) ){
    //makepacket
    makepacket(sendpkt,nextseqnum);
    //sendpacket
    sendpacket(sendpkt);
    // if(base==nextseqnum)//change timer thing
     
    nextseqnum++;
    printf("I am here1\n");
    printf("%s\n",sendpkt.payload);
  }
  else{
    //Refuse 
  }

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
//major change
int checksum = 0;
checksum += packet.seqnum + packet.acknum;
if(checksum == packet.checksum){
      // for(int i=0;i<rl && !=r[packet.seqnum];i++);//can optimize
      if(r[packet.seqnum]!=1)
      r[packet.seqnum]=1;
      if(packet.acknum==base){
        while(r[base]==1){
          base++;
        }
        stoptimer(0);
        starttimer(0,15*winsize);
      }
}
else{
  //do nothing
}
printf("%d\n",base);
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  //change this

starttimer(0,15*winsize);
for(int i=base;i<=nextseqnum-1;i++){
  if(r[i]!=1){
  makepacket(bufferpkt,i);
  sendpacket(bufferpkt);}
}
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    base=1;
    nextseqnum=1;
    winsize=getwinsize();
    bufferseqnum=1;
    starttimer(0,15*winsize);
    rl=0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  //major change
  printf("I am here2\n");
   printf("%s\n",packet.payload);
   int checksum = 0;
for(int i=0;i<20;i++){
  checksum += packet.payload[i];
}
checksum += packet.seqnum + packet.acknum;
if(checksum == packet.checksum){ // If not corrupted and has expected sequence number
      if(packet.seqnum >= rbase && packet.seqnum<=(rbase + rwindow -1)){
        printf("I am here3\n");
              ackpkt.seqnum=packet.seqnum;
            ackpkt.acknum=packet.acknum;
            ackpkt.checksum=ackpkt.seqnum+ackpkt.acknum;
            tolayer3(1,ackpkt);
            rlist[packet.seqnum]=1;
            if(packet.seqnum!=rbase){
              printf("I am here4\n");
              //buffer
              rbuffer[packet.seqnum]=packet;
            }
            else{printf("I am here5\n");
              rbuffer[packet.seqnum]=packet;
              while(rlist[rbase]==1){
                printf("I am here6\n");
                printf("%s\n",rbuffer[rbase].payload);
                strncpy(rcv,rbuffer[rbase].payload,20);
                printf("%s\n",rcv);
                tolayer5(1,rbuffer[rbase].payload);
                rbase++;
              }
            }
      }
      else{
              if(packet.seqnum >= (rbase-rwindow) && packet.seqnum<=(rbase-1)){
                ackpkt.seqnum=packet.seqnum;
                ackpkt.acknum=packet.acknum;
                ackpkt.checksum=ackpkt.seqnum+ackpkt.acknum;
                tolayer3(1,ackpkt);
              }
              else{
                //do nothing
              }
      }
}
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
rbase=1;
rwindow=getwinsize();
ackpkt.seqnum=0;
ackpkt.acknum=0;
}
