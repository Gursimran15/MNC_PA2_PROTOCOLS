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
struct buf{
  string b;
  int r;
  float st;
};
int base,nextseqnum,winsize,bufferseqnum,rbase,rwindow;
map<int, string> buffer;
queue<int> readyq;
map<int,string> rbuffer;
map<int,int> r;
map<int,int> rlist;
map<int,float> sendtime;
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
  //Storing in Buffer
  buffer[bufferseqnum]=message.data;
  bufferseqnum++;
  for(int i=1;i<bufferseqnum;i++){
    cout<<i<<'\n'<<buffer[i]<<'\n';
  }
  cout<<nextseqnum<<'\n'<<buffer[nextseqnum]<<'\n';
   if((nextseqnum) < (base + winsize) ){
    //makepacket
    makepacket(sendpkt,nextseqnum);
    //sendpacket
    sendpacket(sendpkt);
    sendtime[nextseqnum]=get_sim_time();
    readyq.push(nextseqnum);
    if(nextseqnum==1)
      starttimer(0,15.00);
    cout<<"Send\n";
    cout<<nextseqnum<<'\n';
    cout<<base<<'\n';
    cout<<sendpkt.seqnum<<' '<<sendpkt.payload<<'\n';
    cout<<readyq.size();
    nextseqnum++;
  }
  else{
    //Refuse 
  }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{

//Calculating Checksum
int checksum = 0;
 cout<<readyq.size();
checksum += packet.seqnum + packet.acknum;
if(checksum == packet.checksum){ //check for corruption
if(packet.seqnum>=base && packet.seqnum<=(base+winsize-1)){
      stoptimer(0);
      printf("I am here8\n");
      if(r[packet.seqnum]!=1) //marking as received
      r[packet.seqnum]=1;
      if(packet.acknum==base){  // If received an ack for base, slide the window for all received packages
        while(r[base]==1){
          buffer[base]="";
          base++; 
        }
      }
       printf("I am here88\n");
       cout<<packet.seqnum<<'\n';
      while(r[readyq.front()] == 1)
      readyq.pop();
      // if(!readyq.empty()){
      float time = get_sim_time() - sendtime[readyq.front()];
      starttimer(0,15.00-time);
}
}
else{
  //do nothing
}
 cout<<readyq.size();
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
//Resend the packet whose ack is not received
  makepacket(bufferpkt,readyq.front());
  sendpacket(bufferpkt);
  sendtime[readyq.front()]=get_sim_time();
  cout<<readyq.front();
   cout<<readyq.size();
  int x=readyq.front();
  readyq.push(readyq.front());
  readyq.pop();
   while(r[readyq.front()] == 1)
      readyq.pop();
  float time = get_sim_time() - sendtime[readyq.front()];
  starttimer(0,15.00-time);
  }


/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  for(int i=0;i<1000;i++){
    r[i]=0;
  }
    base=1;
    nextseqnum=1;
    winsize=getwinsize();
    bufferseqnum=1;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  cout<<rbase<<'\n';
    cout<<packet.seqnum<<' '<<packet.payload<<'\n';
   int checksum = 0;
for(int i=0;i<20;i++){
  checksum += (int)packet.payload[i];
}
checksum += packet.seqnum + packet.acknum;
if(checksum == packet.checksum){ // If not corrupted and has expected sequence number
      if(packet.seqnum >= rbase && packet.seqnum<=(rbase + rwindow -1)){
              ackpkt.seqnum=packet.seqnum;
            ackpkt.acknum=packet.acknum;
            ackpkt.checksum=ackpkt.seqnum+ackpkt.acknum;
            tolayer3(1,ackpkt); //sending ack for packet in expected window
            rlist[packet.seqnum]=1;
            if(packet.seqnum!=rbase){
              //buffer
              rbuffer[packet.seqnum]=packet.payload;
            }
            else{
              rbuffer[packet.seqnum]=packet.payload;
              do{
                for(int i=0;i<20;i++)
                rcv[i]=rbuffer[rbase][i];
                tolayer5(1,rcv); 
                rbase++;
              }while(rlist[rbase]==1);
            }
      }
      else{  //If not in current window but in any previous window
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
  for(int i=0;i<1000;i++){
    rlist[i]=0;
  }
rbase=1;
rwindow=getwinsize();
ackpkt.seqnum=1;
ackpkt.acknum=1;
}
