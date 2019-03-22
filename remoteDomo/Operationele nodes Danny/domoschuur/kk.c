
//#define test
#ifndef test
#include "Arduino.h"
#else
extern void digitalWrite(int pin,int data);
extern int  digitalRead(int pin);
extern void pinMode(int pin, int mode);
extern void sei();
extern void cli();
#endif


#include "kk.h"

typedef struct
  {
    unsigned long int regdata; /* 26 bits address, group bit, on/off bit and 4 bit channel */
    unsigned char     dimdata; /* 3 bits unused, one bit dimmode, 4 bits dim level */   
  } frame_t;
  
static volatile frame_t       sendbuffer[KK_SENDBUFSIZE];
static volatile unsigned int  sendbufhead=0;
static volatile unsigned int  sendbuflength=0;
static volatile frame_t       recbuffer[KK_RECBUFSIZE];
static volatile unsigned int  recbufhead=0;
static volatile unsigned int  recbuflength=0;

#define KK_ZERO_OLD (0x0303u)  /*10001000 -> 0000001100000011 after bit doubling/reversal */
#define KK_ONE_OLD  (0x033Fu)  /*11101000 -> 0000001100111111 after bit doubling/reversal */
#define KK_ZERO_NEW (0x0033u)  /*101000   ->     000000110011 after bit doubling/reversal */
#define KK_ONE_NEW  (0x0303u)  /*100010   ->     001100000011 after bit doubling/reversal */
//#define KK_ZERO_NEW (0x0033u)  /*1010000   ->   00000000110011 after bit doubling/reversal */
//#define KK_ONE_NEW  (0x0c03u)  /*1000010   ->   00110000000011 after bit doubling/reversal */
#define KK_DIM_NEW  (0x0033u)  /*1010     ->         00110011 after bit doubling/reversal */
#define KK_OFF_OLD  (0x06u)
#define KK_ON_OLD   (0x0Eu)
#define KK_R_ZERO_OLD (0x5u) /* 0101 short long short long */
#define KK_R_ONE_OLD  (0x9u) /* 1001 long short short long */
#define KK_R_ZERO_NEW (0x1u) /* 0001 short short short long */
#define KK_R_ONE_NEW  (0x4u) /* 0100 short long short short */
#define KK_R_DIM_NEW  (0x0u) /* 0000 short short short short */
/* persistent storage for interrupt send/receive handler between interupts */

/* main state
 * SENDING   -> RECEIVING  send buffer empty, last frame sent
 * RECEIVING -> SENDING    send buffer empty and in pause of at least 50T or when receiving garble
 *
 * There is a lot of garble when there is no data being sent. Presumably because of the auto gain of the
 * tranceiver. This may continuously result in invalid data frames. The frame error state is reset once
 * a pause of at least 20T is detected. A valid frame is only copied to the receive buffer once at least
 * MIN_REC_IDENT_FRAMES have been received.
 * Transition to sending state will occur wh
 */
 
#define STATE_IDLE       0
#define STATE_SENDING    1
#define STATE_RECEIVING  2

#define PHASE_DONE       0
#define PHASE_STARTPULSE 1
#define PHASE_DATA       2
#define PHASE_STOPPULSE  3

#define ERR_DATA_OLD                1
#define ERR_DATA_NEW                2
#define ERR_UNEXPECTED_DIM_SEQUENCE 3
#define ERR_INVALID_CMD             4

#define INVALIDFRAME_RD  0
#define INVALIDFRAME_DD  255

#ifdef test
#define HIGH 0x1
#define LOW  0x0
#define INPUT 0x0
#define OUTPUT 0x1
#endif

/* variables used while sending and receiving */
static frame_t       frame;
static frame_t       lastframe;
static unsigned char state = STATE_IDLE; /*states IDLE, SENDING, RECEIVING*/
static unsigned char phase;
static unsigned int  bitstring;
static unsigned char nbit = 0;
static unsigned long rdata;
static unsigned char rnbit;
static unsigned char oldprotocol;
static unsigned char dimmode;
static unsigned char count;
static unsigned char npulse;
static unsigned char data;
static unsigned char lastlevel;
static unsigned char framerepcount=0;
static unsigned char err=0;
static unsigned int  time=0; /* counter incremented every 175 us */
static unsigned int  lastframerxts=0; /* value of time when last frame received was put in frame buffer*/


#ifndef test
static void setuptimer(void)
  {
    TCCR1A = 0;      
    TCCR1B = _BV(WGM12) | _BV(CS10);  /* set mode 4,  no prescaling */
    cli();  
    OCR1A = (F_CPU / 1000000) * 175;  /* 175 us period */
    sei();
    TIMSK1 = _BV(OCIE1A); 
  }

ISR(TIMER1_COMPA_vect)  
{
  static unsigned char overruncheck  = 0;
  static unsigned int  ledblinkcount = 0; 
  /* if called when previous call not completed (interrupt overrun)
   * do not reenter kk_statehandler but turn on led
   */   
  if (overruncheck==0)
    {
      overruncheck = 1;
      kk_statehandler();
      ledblinkcount++;
#ifdef KK_USELED
      if (ledblinkcount==(50000/175)) /* 0.05s on */
        digitalWrite(KK_LEDPIN,0);
      if (ledblinkcount==(1000000/175))/* one second period*/
        {
          ledblinkcount=0;
          digitalWrite(KK_LEDPIN,1);
        }  
      overruncheck = 0;
    }
  else
    {
      /* switch on LED */
      digitalWrite(KK_LEDPIN,1);
      ledblinkcount= 1000000/175 + 1; /* timer interrupt overrun wil turn on led for a longer time */
#endif 
    }
}
#else
static void setuptimer(void)
  {
  }
#endif
  
void kk_init(void)
  {
    state     = STATE_RECEIVING;
    phase     = PHASE_STARTPULSE;
    npulse    = 3; /* low -> high -> low -> high */
    lastlevel = 0;
    count     = 0;
    data      = 0;
    framerepcount     = 0;
    lastframe.regdata = INVALIDFRAME_RD;
    lastframe.dimdata = INVALIDFRAME_DD;
    pinMode(KK_RXPIN,INPUT);
    if (KK_TXPIN==KK_RXPIN) /* input and output on same pin, KK_MODEPIN used to swich between in and output*/
      {
        pinMode(KK_MODEPIN,OUTPUT);
        digitalWrite(KK_MODEPIN,0);
      }
    else 
      {
        pinMode(KK_TXPIN,OUTPUT);
      }           
    setuptimer();  
#ifdef KK_USELED    
    pinMode(KK_LEDPIN,OUTPUT);
#endif    
  }

int  kk_available(void)
  {
    return(recbuflength>0);
  }
  
int  kk_sendbufempty(void)
  {
    return(sendbuflength==0);
  }
int  kk_sendbuffull(void)
  {
    return(sendbuflength==KK_SENDBUFSIZE);
  }
  

void kk_send(unsigned long  address, unsigned char unit, unsigned char on, unsigned char dimlevel)
  {
    frame_t sframe;    
    int tail;       

    /* wait for empty slot in send buffer */
    while (sendbuflength>=KK_SENDBUFSIZE);
    
    /* encode frame */
    sframe.regdata = address << 6;
    if (unit==255) /* group command */
      sframe.regdata |= 0x00000020ul;
    else
      sframe.regdata |= (unsigned long int)unit;        
    if (on==1)
      sframe.regdata |= 0x00000010ul;
    if (on==2)
      sframe.dimdata = 0x80 | dimlevel;
    else  
      sframe.dimdata = 0;
    
    /* insert frame in send buffer */
    cli();
    tail = sendbufhead + sendbuflength; 
    if (tail>=KK_SENDBUFSIZE) /* wrap */
      {
         tail = tail - KK_SENDBUFSIZE;
      }
    sendbuffer[tail].regdata= sframe.regdata;  
    sendbuffer[tail].dimdata= sframe.dimdata;  
    if (sendbuflength<KK_SENDBUFSIZE)
      {
        sendbuflength++;
      }
    sei();
  }      

int  kk_receive(unsigned long *address, unsigned char *unit, unsigned char *on, unsigned char *dimlevel)
  {
    int res = 0;
    frame_t rframe;
    if (recbuflength>0)
      {
        res = 1;
        cli();
        rframe.regdata = recbuffer[recbufhead].regdata;
        rframe.dimdata = recbuffer[recbufhead].dimdata;
        recbufhead++;
        if (recbufhead==KK_RECBUFSIZE) /* wrap buffer */
          recbufhead = 0;
        recbuflength--;
      
        sei();
        if (address) *address = (rframe.regdata & 0xFFFFFFC0ul) >> 6; /* 26 highest bits are address */
        if (unit)
          {
            *unit = (rframe.regdata & 0x0000000Ful); /* lowest nibble is unit */
            if (rframe.regdata & 0x00000020ul) /* group bit is set */
              *unit = 255;
          }
        if (on)
          {
            *on =  (rframe.regdata & 0x00000010ul) != 0;
            if (rframe.dimdata & 0x80)
              *on = 2; /*set dim level */
          }  
        if (dimlevel) *dimlevel =  (rframe.dimdata & 0x0F);
      }
    return(res);
  }

void kk_statehandler(void)
  {
    time++;     
    if (time==lastframerxts) /* wraparound, happens ~12s after last frame received */
      {
        /* reset lastframe */
        lastframe.regdata = INVALIDFRAME_RD;
        lastframe.dimdata = INVALIDFRAME_DD;
      }
    if (state==STATE_SENDING)
      {
        /*if the current bit pulse string is empty, fill it with the next bit */
        if (nbit==0)
          {
            if (phase==PHASE_DONE)
              {
                if (count>0)
                  {
                    /* repeat last frame */
                     count--;
                     phase       = PHASE_STARTPULSE;
                     dimmode     = (frame.dimdata != 0); /* was cleared*/
                  }
                else if (sendbuflength != 0)
                  {
                    /* check if there is a frame in the send buffer and if so, get it */
                     frame.regdata = sendbuffer[sendbufhead].regdata;
                     frame.dimdata = sendbuffer[sendbufhead].dimdata;
                     sendbufhead++;
                     if (sendbufhead==KK_SENDBUFSIZE) /* wrap buffer */
                       {
                         sendbufhead = 0;
                       }
                     sendbuflength--;
                     count       = KK_SEND_REPEAT-1;
                     oldprotocol = ((frame.regdata & 0xfffffc00ul)==0);/* 0-15 */
                     dimmode     = (frame.dimdata != 0);
                     phase       = PHASE_STARTPULSE;
                     /* printf("%ld\n",(unsigned long)frame.regdata);*/
                   }
                 else
                   {
                     /* nothing more to send,  switch to receiving mode*/   
                     state     = STATE_RECEIVING;
                     phase     = PHASE_STARTPULSE;
                     npulse    = 3; /* low -> high -> low -> high */
                     lastlevel = 0;
                     count     = 0;
                     data      = 0;
                     framerepcount     = 0;
                     lastframe.regdata = INVALIDFRAME_RD;
                     lastframe.dimdata = INVALIDFRAME_DD;
                     if (KK_TXPIN==KK_RXPIN)
                       {
                         pinMode(KK_RXPIN,INPUT); /* first switch port to input */
                         digitalWrite(KK_MODEPIN,0); /* then switch tranceiver */
                       }  
                     return;
                   }  
               }    
            switch (phase)
              {
               case PHASE_STARTPULSE:
                   /* create bit string to be sent */
                   bitstring = 3;                  /* one pulse of two cycles*/
                   nbit  = (oldprotocol)? 8 : 20;  /* followed by one or nine zeros*/
                   /* This completes the (preparation) of the start pulse phase,
                    * though it is still to be transmitted. The state engine can now
                    * transfer th the next phase. This includes initialisation of the data for this state */
                   phase = PHASE_DATA;
                   if (oldprotocol)
                     {
                        /* when using the old protocol, transmission is 12 bit, MSB first!!!
                         * encode 4 nibbles, low to high address, unit and command
                         * then reverse the entire word as we are shifting out to the left.
                         */
                       rdata = ((frame.regdata & 0x000003c0ul) >> 6)      /* Address */
                             | ((frame.regdata & 0x0000000ful) << 4)      /* unit */
                             |(((frame.regdata & 0x00000010ul)!= 0) ? (KK_ON_OLD << 8): (KK_OFF_OLD << 8));
                       rnbit  = 12;  /* entire message has been encoded in rdata*/
                     }
                   else
                     {
                       rdata  = frame.regdata;
                       rnbit  = 32;  
                     }
                   break;
               case PHASE_DATA:
                   if (oldprotocol)
                     { 
                        /*old protocol is shift out right lsb first*/
                        bitstring = (rdata & 1) ? KK_ONE_OLD : KK_ZERO_OLD;
                        nbit      = 16;
                        rdata >>= 1; 
                     }
                   else
                     {  
                       if ((rnbit==5) && dimmode)
                         {
                           bitstring = KK_DIM_NEW;
                           nbit      = 8;
                         }  
                       else
                         {
                           bitstring = (rdata & 0x80000000ul) ? KK_ONE_NEW : KK_ZERO_NEW;
                           nbit      = 12;
                         }
                       rdata <<= 1;
                      }    
                   
                   if (--rnbit == 0)
                     {
                       if (!oldprotocol && dimmode)
                         {
                           /* append dim level */     
                           rdata   = (unsigned long)(frame.dimdata & 0x0f) << 28;
                           rnbit   = 4;
                           dimmode = 0;     
                         }
                       else          
                           /* done, go to next phase */
                           phase = PHASE_STOPPULSE;
                     }
                   break;
               case PHASE_STOPPULSE:
                   bitstring = (oldprotocol)? 0 : 3;
                   nbit      = 66;
                   phase     = PHASE_DONE;
              }
          }
        /* if bitstring is still empty here, there is nothing left to send */
        if (nbit!=0)
          {  /* this is where the actual signal is generated, */
             digitalWrite(KK_TXPIN,((bitstring & 1u) != 0));
             bitstring >>= 1;
             nbit--;
          }
      }
    if (state==STATE_RECEIVING)
      {
        char level;
        char bit;
        /* here is where we receive the next pulse string
         * Possibilities:
         *   - in the middle of receiving a sequence
         *   - Just finished a full frame and waiting for the next to start or 
         *     or if that does not happen, to go to idle mode
         *   - in a frame error state, waiting for a long enough pause indicating a frame end 
         */
        level = digitalRead(KK_RXPIN); /* read signal level */
        if (level!=lastlevel) /* did we make a level transition */
          {
            /* if so, was it long or short */
            data = (data << 1) | (count > 4);
            if (count>12)
              data = 255; /* used for space length after start pulse and 
                           * create errors if pulses/spaces are too long */
            npulse--;
            count = 0; 
            lastlevel=level;
          }
        if ((count == 20) && (phase==PHASE_DATA))
          {
            /* in the old protocol, there is no stop pulse */
            data = (data << 1) | 1;
            npulse=0;
          }      
         
        if ((count >= 21) && (level==0))
          {
            /* current frame has presumably ended, reset frame error */ 
            phase     = PHASE_STARTPULSE;
            npulse    = 3; /* level is still low, so waiting for up down up to finish start pulse*/
            data      = 0;
            err       = 0;
            //Serial.print("r");
          }   

        /* We may have something in the send buffer.
         * But don't switch to sending when we are in the middle of receiving a series of frames
         */  
        if ((sendbuflength != 0) && (err || ((count >= 100) && (level==0))))
          {
            state     = STATE_SENDING;
            phase     = PHASE_DONE;  /* same as previous frame just completed */
            nbit      = 0;           /* needed to specify lowest level send buffer is empty*/
            count     = 0;           /* used in both modes */
            if (KK_TXPIN==KK_RXPIN)
              {
                digitalWrite(KK_MODEPIN,1); /* first switch tranceiver to transmit mode */
                pinMode(KK_TXPIN,OUTPUT); /* then switch the pin to output */
              }
            return;
          }

        if (count!=255) 
          count++;  

        /* If npulse  is zero, process data, reload data and npulse for the next sequence*/
        if ((npulse==0) && (err==0))
          {
            switch (phase)
              {
                case PHASE_STARTPULSE:
                  oldprotocol = (data!=255); /* new protocol has extra long space after start pulse*/
                  phase   = PHASE_DATA;
                  npulse  = 4;
                  rnbit   = oldprotocol ? 12 : 32;
                  rdata   = 0;
                  dimmode = 0;
                  break;
                case PHASE_DATA:
                  if (oldprotocol)
                    {
                      if      (data==KK_R_ZERO_OLD)
                          bit = 0; 
                      else if (data==KK_R_ONE_OLD)
                          bit = 1;
                      else
                        {
                          bit = 0; 
                          err = ERR_DATA_OLD; 
                        }  
                      rdata >>= 1;
                      if (bit)
                        {
                          rdata |= 0x80000000ul;  
                        }         
                    }
                  else
                    {
                      if      (data==KK_R_ZERO_NEW)
                          bit = 0; 
                      else if (data==KK_R_ONE_NEW)
                          bit = 1;
                      else if (data==KK_R_DIM_NEW)
                        {
                          /* We received the dim sequence short1-short0-short1-short0
                           * This should only happen at the fifth last bit, as alternative to the on-off bit
                           */
                          bit     = 0;
                          dimmode = 1;
                          if (rnbit!=5)
                            {
                              err=ERR_UNEXPECTED_DIM_SEQUENCE;
                            }
                        }
                      else 
                        {
                          bit = 0;
                          err = ERR_DATA_NEW; 
                        }  
                      rdata <<= 1;
                      if (bit)
                        {
                          rdata |= 0x00000001ul;  
                        }     
                    }   

                  rnbit--;      
                    
                  if(rnbit==0) /* received all data unless dimlevel is still to follow */
                    {  
                      /* Possibilities:
                       * Old protocol: we are done
                       * New protocol:
                       *  - dimmode == 0 : we we have received the full 32 bit of the frame                      
                       *  - dimmode == 1, : we we have received the first 32 bit of the frame, dim level still to follow
                       *  - dimmode == 2  : we have received the last 4 bit containing the dim level 
                       */
                      if (oldprotocol)
                        { 
                           unsigned char addr;
                           unsigned char unit;
                           unsigned char cmd;
                           unsigned char on;
                           rdata >>= (32-12); /* make the firstly received bit the LSB */
                           cmd  = (rdata & 0xf00u) >> 8;
                           unit = (rdata & 0x0f0u) >> 4;
                           addr =  rdata & 0x00fu;
                           if (cmd==KK_ON_OLD)
                             {
                               on = 1;
                             }
                           else if  (cmd == KK_OFF_OLD)
                             {
                               on = 0;
                             }
                           else
                             {
                               err=ERR_INVALID_CMD;
                               on = 0;  
                             }
                           frame.regdata = (addr << 6) | (unsigned long int) unit;  
                           if (on)
                             frame.regdata |= 0x00000010ul;
                           frame.dimdata = 0;        
                        }
                      else if (dimmode!=2)
                        {
                           frame.regdata = rdata;
                           frame.dimdata = 0; /* will be overwritten later if dimmode is 1*/
                           if (dimmode==1)
                             {
                               rnbit   = 4;
                               rdata   = 0;
                               dimmode = 2;     
                             } 
                        }
                      else
                        {
                           frame.dimdata = rdata | 0x80; /* only the top 4 bits were filled yet*/
                        }
                    }
                  if (rnbit==0) /* did it stay zero, then store the frame in the receive buffer*/
                    {    
                      /* store data in receive, buffer, if buffer is full, discard oldest entry */
                      if ((frame.regdata == lastframe.regdata) && (frame.dimdata == lastframe.dimdata))
                        {
                          if (framerepcount!=255)
                             framerepcount++;
                        } 
                      else
                        framerepcount = 1;  
                      if (framerepcount==KK_MIN_REC_IDENT_FRAMES) 
                        { 
                          int tail;       
                          tail = recbufhead + recbuflength; 
                          if (tail>=KK_RECBUFSIZE) /* wrap */
                            {
                               tail = tail - KK_RECBUFSIZE;
                            }
                          recbuffer[tail].regdata= frame.regdata;  
                          recbuffer[tail].dimdata= frame.dimdata;  
                          if (recbuflength<KK_RECBUFSIZE)
                            {
                              recbuflength++;
                            }
                          lastframerxts = time; /* timestamp it */
                        }        
                      //printf("RF: %8.8lx %2.2x%\n",frame.regdata,frame.dimdata);  
                      lastframe.regdata=frame.regdata;
                      lastframe.dimdata=frame.dimdata;
                      /* Either we have just started the stop pulse (new protocol)
                       * or we are already waiting for the next frame (old protocol)
                       *  -first case level is high and we are waiting for 
                       *      low----high(start)-low-high(data)
                       * - second case level is low and we are waiting for 
                       *             high(start)-low-high(data)
                       */
                      phase  = PHASE_STARTPULSE;
                      npulse = 3 + level;     
                    }
                  else
                    npulse=4;  
                  break;
              } /* switch */
            data = 0;      
          }  /* if npulse== 0*/
      } /* state */ 
  }
