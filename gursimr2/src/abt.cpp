#include "../include/simulator.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
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
char buffer[20000];
int count;
int flag;
char lastsent[20];
int msgdelivered;
int incomingcount=0;
char lastincomingl5[20];
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
int makechecksum(pkt pack){
  int c = pack.acknum + pack.seqnum;
    for(int i=0;i<20;i++){
      if(strlen(pack.payload)!=0)
        c=c+ (int)pack.payload[i];
    }
}
/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message)
{
  printf("Message as it is=%s\n",message.data);
  if(strncmp(message.data,lastsent,sizeof(message.data))!=0){
            if(strncmp(message.data,lastincomingl5,sizeof(message.data))!=0){
                    incomingcount++;
                    for(int i=0;i<sizeof(message.data);i++)
                    buffer[i+ (incomingcount-1)*20]=message.data[i];
                    strncpy(lastincomingl5,message.data,strlen(lastincomingl5));
                     printf("Message after buffer but inside lastincoming compare IF=%s\n",message.data);
                     printf("%d \n",incomingcount);
                    }
            printf("Message after buffer=%s\n",message.data);
            printf("%d \n",incomingcount);
            printf("%d \n",count);
            //  strncat(buffer,message.data,sizeof(message.data));
            //  while(flag!=0)
            //  usleep(10000);
            if(flag==0 ||(msgdelivered && strncmp(lastsent,packtob.payload,sizeof(lastsent))==0 )){ // Condition to deal with skipping messages due to delayed ack
                    count++;
                    printf("%d \n",count);
                    printf("%d \n",packtob.checksum);
                    printf("%d \n",packtob.seqnum);
                    printf("%d \n",packtob.acknum);
                    packtob.seqnum=changeseqno(packtob.seqnum);
                      packtob.acknum=changeackno(packtob.acknum);
                  for(int i=0;i<20;i++){
                    
                    packtob.payload[i]=buffer[(count-1)*20 + i];
                  }
                  }
 }
 packtob.checksum=makechecksum(packtob);
 printf("%d \n",count);
  printf("%d \n",packtob.checksum);
  printf("%d \n",packtob.seqnum);
  printf("%d \n",packtob.acknum);
  // flag=1;
  stoptimer(0);
 tolayer3(0,packtob);
 starttimer(0,15);
 msgdelivered=0;
 flag=1; //for first send
 strncpy(lastsent,packtob.payload,sizeof(lastsent));
  printf("%s","Message sent and timer started\n");
 printf("%s %s\n",packtob.payload,lastsent);
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
  // if(packtob.seqnum == 1){
  //   ackfromb.acknum = 0;
  // }
  // else{
  //   ackfromb.acknum = 1;
  // }
  // ackfromb.checksum = packtob.seqnum + ackfromb.acknum;
  //  if(packet.acknum != ackfromb.acknum){
  //  }
  // if(packet.checksum != ackfromb.checksum){
  // }
  ackfromb.seqnum = packtob.seqnum;
  ackfromb.acknum = packtob.acknum;
  int ic = makechecksum(packet); //Received
  ackfromb.checksum = makechecksum(ackfromb); //Excepted
  if(packet.checksum == ic){
    if(packet.checksum==ackfromb.checksum && packet.seqnum==ackfromb.seqnum && packet.acknum==ackfromb.acknum){
       stoptimer(0);
    //  flag=0;
     printf("%sAcknowledgement received for \n",packtob.payload);
     msgdelivered = 1;
    }
    else{
      stoptimer(0);
      A_timerinterrupt();
      //NotExpected
    }
  }
  else{
    //Corrupted
    stoptimer(0);
      A_timerinterrupt();
  }
  // if(packet.payload[0]=='C'){
  //   stoptimer(0);
  //   A_timerinterrupt();
  // }
  // if(packet.checksum == ackfromb.checksum){ //if not corrupted
  //   if(packet.acknum == ackfromb.acknum){
    
  //   }
  // }
}
/* called when A's timer goes off */
void A_timerinterrupt()
{
  msg m;
  strncpy(m.data,lastsent,sizeof(m.data));
  A_output(m);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  count=0;
packtob.seqnum = 1;
packtob.acknum = 0;
packtob.checksum = 0;
flag=0;
msgdelivered =0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
 strncpy(packfroma.payload,packet.payload,sizeof(packfroma.payload));
 packfroma.checksum=makechecksum(packet);
 printf("%d %d\n",packet.checksum,packfroma.checksum);
 printf("%d %d\n",packet.seqnum,packfroma.seqnum);
  printf("%d %d\n",packet.acknum,packfroma.acknum);
//  for(int i=0;i<20;i++){
//    packfroma.payload[i]=packet.payload[i];
//    packfroma.checksum += (int)packfroma.payload[i];
//  } 
//  printf("%s","before checksum\n");
//  packfroma.checksum += packfroma.seqnum + packfroma.acknum;
//  printf("%s","after checksum\n");
 if(packet.checksum != packfroma.checksum){ // If corrupted
     acktoa.seqnum = packfroma.seqnum;
     acktoa.acknum = packfroma.acknum;
     acktoa.checksum = acktoa.seqnum + acktoa.acknum;
     strncpy(acktoa.payload,packfroma.payload,strlen(acktoa.payload));
     tolayer3(1,acktoa);
 }//Above code will give Incorrect checksum for ack at sender's side
 else{
   if(packet.seqnum != packfroma.seqnum){ // for duplicate or Out of Order - Not Expected Packet
     acktoa.seqnum = packfroma.acknum;
     acktoa.acknum = packfroma.seqnum;
     bzero(acktoa.payload, sizeof(acktoa.payload));
     acktoa.checksum = makechecksum(acktoa);
     tolayer3(1,acktoa);
 }
 else{//Correct Checksum and Expected Packet
   acktoa.seqnum = packfroma.seqnum;
     acktoa.acknum = packfroma.acknum;
     bzero(acktoa.payload, sizeof(acktoa.payload));
     acktoa.checksum = makechecksum(acktoa);
     tolayer3(1,acktoa);
    tolayer5(1,packfroma.payload);
    packfroma.seqnum=changeseqno(packfroma.seqnum);
    packfroma.acknum=changeackno(packfroma.acknum);

 }

 }
 
 
 printf("%s","After Sending Ack\n");
 
//  if(packet.checksum == packfroma.checksum){ 
//    printf("%s","correct checksum\n");
//    //if not corrupted
//    if(packet.seqnum == 0){
//      acktoa.seqnum = packfroma.seqnum;
//      acktoa.acknum = 1;
//      acktoa.checksum = acktoa.seqnum + acktoa.acknum;
//       // acktoa.payload[0] = 'A';
//      tolayer3(1,acktoa);
//     tolayer5(1,packfroma.payload);
//    }else{
//      acktoa.seqnum = packfroma.seqnum;
//      acktoa.acknum = 0;
//      acktoa.checksum = acktoa.seqnum + acktoa.acknum;
//       // acktoa.payload[0] = 'A';
//      tolayer3(1,acktoa);
//      tolayer5(1,packfroma.payload);
//    }
   printf("I am here");
    printf("%d \n",packfroma.seqnum);
  printf("%d \n",packfroma.acknum);
  // if(packfroma.seqnum==1){
  //     packfroma.seqnum=0;
  //   }
  //   else{
  //     packfroma.seqnum=1;
  //   }
  //    if(packfroma.acknum==1){
  //     packfroma.acknum=0;
  //   }
  //   else{
  //     packfroma.acknum=1;
  //   }
//  }

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
//  count=0;
packfroma.seqnum = 0;
packfroma.acknum = 1;
packfroma.checksum=0;
}
