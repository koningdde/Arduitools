#ifndef __KK_H
#define __KK_H
#ifdef __cplusplus
extern "C"{
#endif
/* kk_init: initialises the driver, sets up the timer interrupt */
extern void kk_init(void);

/* kk_end:  stops the timer interrupt*/
extern void kk_end(void);

/* kk_available: check is there is something in the receive buffer */
extern int  kk_available(void);

/* kk_sendbufempty: send buffer buffer empty? */
/* kk_sendbuffull:  space in the send buffer? */
extern int  kk_sendbufempty(void);
extern int  kk_sendbuffull(void);

/* kk_send: put command in send buffer. if buffer is full, it waits until an empty slot is available
 *   address : adress of the unit the command is send to. If adress is smaller than 16, old protocol is used.
 *   channel : 4 bit channel code of the device. (0-15), 255 is group 
 *   on      : 0 means off, 1 means on, and 2 means set dim level
 *   dimlevel: dimlevel to be set
 * Note that group commands and setting dim level only works for devices that implement the new protocol  
 */
extern void kk_send   (unsigned long  address, unsigned char  unit, unsigned char on, unsigned char dimlevel);

/* kk_receive: if available, reads data from input buffer. If not, returns immediately
 * Arguments:   
 *   address : adress of the unit the command is send to. If adress is smaller than 16, old protocol is used.
 *   channel : 4 bit channel code of the device. (0-15), 255 is group 
 *   on      : 0 means off, 1 means on, and 2 means set dim level
 *   dimlevel: dimlevel to be set
 * Returns 0 : no data, 1 : data
 * Note that group commands and setting dim level only works for devices that implement the new protocol  
 */
extern int  kk_receive(unsigned long *address, unsigned char *unit, unsigned char *on, unsigned char *dimlevel);

/* kk_statehandler: exported mainly for test purposes. to be called from timer ISR every 180 microseconds */
extern void kk_statehandler(void);


/* Compile time configuration */
#define KK_SENDBUFSIZE           10 /* ten frames input buffer */
#define KK_RECBUFSIZE            10 /* ten frames output buffer */
#define KK_SEND_REPEAT           5  /* send frame 5 times. May be overkill*/
#define KK_MIN_REC_IDENT_FRAMES  2  /* receive at least two identical frames to qulaify input */

#define KK_RXPIN   3 /* Arduino input pin, for receiver */
#define KK_TXPIN   5 /* Arduino output pin for transmitter */
#define KK_MODEPIN 8 /* only used if KK_RXPIN == KK_TXPIN */
#define KK_USELED    /* Led is used for diagnostic purposes */
#define KK_LEDPIN 13 /* Arduino pin for LED */ 

#define KK_TIMER TIMER1 /* the only one supported now */


#ifdef __cplusplus
} // extern "C"
#endif
#endif
