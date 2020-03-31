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

/* called from layer 5, passed the data to be sent to other side */
int base,nextseqnum,winsize,maxseqnum,bufferseqnum,expectedseqnum;
map<int, string> buffer;
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
void A_output(struct msg message)
{
  if(bufferseqnum <= maxseqnum){ //into buffer
  buffer[bufferseqnum]=message.data;
  bufferseqnum++;
  }
  else{
    bufferseqnum=1;
  }
  if((nextseqnum % maxseqnum) < (base + winsize) % maxseqnum){
    //makepacket
    makepacket(sendpkt,nextseqnum);
    //sendpacket
    sendpacket(sendpkt);
    if(base==nextseqnum)
      starttimer(0,30);
    nextseqnum++;
  }
  else{
    //Refuse 
  }
  if(nextseqnum > maxseqnum){ // max seq number
    nextseqnum = 1;
  }
  if(base > maxseqnum){
    base=1;
  }

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
int checksum = 0;
// for(int i=0;i<20;i++){
//   checksum += packet.payload[i];
// }
checksum += packet.seqnum + packet.acknum;
if(checksum == packet.checksum){
      base = packet.acknum + 1;
      if(base==nextseqnum)
        stoptimer(0);
      else
      {
        stoptimer(0);
        starttimer(0,30);
      }
}
else{
  //do nothing
}
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
starttimer(0,30);
for(int i=base;i<=nextseqnum-1;i++){
  makepacket(bufferpkt,i);
  sendpacket(bufferpkt);//working on timeout send
}
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
base=1;
nextseqnum=1;
winsize=getwinsize();
maxseqnum = 2*winsize;
bufferseqnum=1;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
 int checksum = 0;
for(int i=0;i<20;i++){
  checksum += packet.payload[i];
}
checksum += packet.seqnum + packet.acknum;
if(checksum == packet.checksum && packet.seqnum==expectedseqnum){ // If not corrupted and has expected sequence number
tolayer5(1,packet.payload);
ackpkt.seqnum=expectedseqnum;
ackpkt.acknum=expectedseqnum;
ackpkt.checksum=ackpkt.seqnum+ackpkt.acknum;
tolayer3(1,ackpkt);
expectedseqnum++;
}
else{ // Default
tolayer3(1,ackpkt);
}
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
expectedseqnum=1;
ackpkt.seqnum=0;
ackpkt.acknum=0;
}
