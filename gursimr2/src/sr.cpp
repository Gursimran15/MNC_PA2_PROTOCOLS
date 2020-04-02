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
struct interrupt{
  int i;
  int pseq;
};
int b=-1;
map<int,interrupt> it;
int base,nextseqnum,winsize,bufferseqnum,rbase,rwindow;
map<int, string> buffer;
map<int,pkt> rbuffer;
map<int,int> r;
map<int,int> rlist;
map<int,float> sendtime;
int intseq;
map <int,int> aftertime;
int rl;
float starttime;
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
  printf("%f\n",get_sim_time());
  //Storing in Buffer
  buffer[bufferseqnum]=message.data;
  bufferseqnum++;
   if((nextseqnum) < (base + winsize) ){
    //makepacket
    makepacket(sendpkt,nextseqnum);
    //sendpacket
    sendpacket(sendpkt);
    it[nextseqnum].i=0;
    printf("%f\n",get_sim_time());
    if(nextseqnum==1){
    starttimer(0,15);
    starttime=get_sim_time();
    }
    sendtime[nextseqnum]=get_sim_time() - starttime;
    printf("%f\n",get_sim_time());
     printf("st %f\n",sendtime[nextseqnum]);
    printf("st %f\n",sendtime[nextseqnum]+15);
    printf("%f\n",get_sim_time());

    nextseqnum++;
    printf("I am here1\n"); //for debug
    printf("%s\n",sendpkt.payload); //for debug
  }
  else{
    //Refuse 
  }

  // for(int i=base;i<=base+winsize;i++)
  // if(sendtime[i]+15>get_sim_time()){
  //   if(r[i]!=1){
  //   intseq = i;
  //   A_timerinterrupt();
  //   }
  // }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{

//Calculating Checksum
int checksum = 0;
checksum += packet.seqnum + packet.acknum;
if(checksum == packet.checksum){ //check for corruption
      if(r[packet.seqnum]!=1) //marking as received
      r[packet.seqnum]=1;
      if(packet.acknum==base){  // If received an ack for base, slide the window for all received packages
        while(r[base]==1){
          base++; 
          b=-1;
        }
        // stoptimer(0);   // Stop and Restart the timer whenever base is changed
        // starttimer(0,15*winsize); // Timer for the whole window so that we give enough time for all packets
      }
      float time=get_sim_time()-starttime;
      printf("1 %f\n",get_sim_time());
      stoptimer(0);
      if(it[base].i == 0){
      starttimer(0,sendtime[base]+15-time);
       printf("1 %f\n",sendtime[base]+15-time);
      }else if(it[base].i==1){
        b++;
        int t,j;
        for(t=0,j=base+1;j<=it[base].pseq;j++)
        aftertime[t++]=j;
        aftertime[t]=base;
        printf("2 %f\n",get_sim_time());
        starttimer(0,sendtime[aftertime[b]]+15-time);
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
//Resend the packet whose ack is not received
  printf("1 %f\n",get_sim_time());
  makepacket(bufferpkt,base);
  sendpacket(bufferpkt);
  sendtime[base]=get_sim_time()-starttime;
  it[base].i=1;
  it[base].pseq = nextseqnum -1;
  float time=get_sim_time() - starttime;
  starttimer(0,sendtime[base + 1]+15-time);
  printf("2 %f\n",sendtime[base + 1]+15-time);
  printf("2 %f\n",get_sim_time());
  }


/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
    base=1;
    nextseqnum=1;
    winsize=getwinsize();
    bufferseqnum=1;
     // Starting Timer for the whole Window, so that there is enough time for all packets
    rl=0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  printf("I am here2\n"); //for debug
   printf("%s\n",packet.payload); //for debug
   int checksum = 0;
for(int i=0;i<20;i++){
  checksum += packet.payload[i];
}
checksum += packet.seqnum + packet.acknum;
if(checksum == packet.checksum){ // If not corrupted and has expected sequence number
      if(packet.seqnum >= rbase && packet.seqnum<=(rbase + rwindow -1)){
        printf("I am here3\n"); //for debug
              ackpkt.seqnum=packet.seqnum;
            ackpkt.acknum=packet.acknum;
            ackpkt.checksum=ackpkt.seqnum+ackpkt.acknum;
            tolayer3(1,ackpkt); //sending ack for packet in expected window
            rlist[packet.seqnum]=1;
            if(packet.seqnum!=rbase){
              printf("I am here4\n"); //for debug
              //buffer
              rbuffer[packet.seqnum]=packet;
            }
            else{printf("I am here5\n"); //for debug
              rbuffer[packet.seqnum]=packet;
              while(rlist[rbase]==1){
                printf("I am here6\n");//for debug
                printf("%s\n",rbuffer[rbase].payload); //for debug
                strncpy(rcv,rbuffer[rbase].payload,20); //for debug
                printf("%s\n",rcv); //for debug
                tolayer5(1,rbuffer[rbase].payload); // Sending all received packets to layer5
                rbase++;
              }
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
rbase=1;
rwindow=getwinsize();
ackpkt.seqnum=0;
ackpkt.acknum=0;
}
