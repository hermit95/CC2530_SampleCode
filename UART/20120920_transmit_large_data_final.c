
/*----------------------------------------------------------------------------
 *  Demo Application for SimpliciTI
 *
 *  L. Friedman
 *  Texas Instruments, Inc.
 *----------------------------------------------------------------------------
 */
/******************************************************************************************

  Copyright 2007-2009 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights granted under
  the terms of a software license agreement between the user who downloaded the software,
  his/her employer (which must be your employer) and Texas Instruments Incorporated (the
  "License"). You may not use this Software unless you agree to abide by the terms of the
  License. The License limits your use, and you acknowledge, that the Software may not be
  modified, copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio frequency
  transceiver, which is integrated into your product. Other than for the foregoing purpose,
  you may not use, reproduce, copy, prepare derivative works of, modify, distribute,
  perform, display or sell this Software and/or its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS?  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY
  WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
  IN NO EVENT SHALL TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE
  THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY
  INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST
  DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY
  THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/
#define INDICATOR_TIME_OUT 250;

#include <stdio.h>
#include "bsp.h"
#include "mrfi.h"
#include "nwk_types.h"
#include "nwk_api.h"
#include "nwk_pll.h"
#include "bsp_leds.h"
#ifdef MRFI_CC430
  #include "uart_intfc_cc430.h"
#else
  #include "uart_intfc.h"
#endif



//static int8_t  sSample;
static void itoa_int16(int16_t n,unsigned char s[]);
static void reverse(unsigned char s[]);

size_t strlen(const unsigned char * str);
size_t strlen_b(const signed char * str);

const int DEVICE_TYPE_LINK = 0;
const int DEVICE_TYPE_LISTEN = 1;

int monitor(void);

void main1 (void);
void main (void)
{
  main1();        
}

void main1 (void)
{
  uint8_t msg[MAX_APP_PAYLOAD];
  uint8_t listen_transmit_on = 0;
  
  int16_t link_count=0;
  int16_t listen_count=0;
  int16_t listen_count_tmp=0;
  int16_t number = 10000;
  //int8_t result_display=0;//0=ÁÙ¨S 1=¦³
  
  int16_t listen_receive_count=0;
  int16_t link_receive_count=0;
  int8_t display_vaule=0;
  
  
  /* holds length of current message */
  uint8_t len; 
  
  /* the link token */
  linkID_t LinkID = 0;
  
  /* the transmit and receive buffers */
  uint8_t tx[MAX_APP_PAYLOAD];
    //rx[MAX_APP_PAYLOAD], tx[MAX_APP_PAYLOAD];

  /* holds led indicator time out counts */
  uint16_t led_tmr;
  /*Device type*/
  int8_t device_type=-1;
  /*start*/

  
  

  BSP_Init( );

  SMPL_Init( NULL );
  
  uart_intfc_init( );

  /* turn on the radio so we are always able to receive data asynchronously */
  SMPL_Ioctl( IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_RXON, NULL );
  
  /* turn on LED. */
  BSP_TURN_ON_LED1( );

#ifdef  LINK_TO
  {
    
    
    uint8_t cnt = 0;
    tx_send_wait( "Linking to...\r\n", 15 );
    while (SMPL_SUCCESS != SMPL_Link(&LinkID))
      if( cnt++ == 0 )
      {
        /* blink LED until we link successfully */
        BSP_TOGGLE_LED1( );
      }
    device_type=DEVICE_TYPE_LINK;
  }
#else // ifdef LINK_LISTEN
  tx_send_wait( "Listening for Link...\r\n", 23 );
  while (SMPL_SUCCESS != SMPL_LinkListen(&LinkID))
  {
    /* blink LED until we link successfully */
    BSP_TOGGLE_LED1( );
  }
  device_type=DEVICE_TYPE_LISTEN;
#endif

  tx_send_wait( "Link Established!\r\nReady...\r\n", 29 );
  
  /* turn off the led */
  BSP_TURN_OFF_LED1( );
  
  T1CC0L = 31250 & 0xFF;            //±N50000§C8¦ì¤¸¼g¤J T1CC0L       
  T1CC0H = ((31250 & 0xFF00) >> 8);// ±N50000°ª8¦ì¤¸¼g¤J T1CC0H  
    
  T1CTL = 0x0F;                    //¤ÀÀW128¦¸  ±Ä¥ÎUP-DOWN­p¼Æ¤è¦¡ //p118
  
  
    
  main_loop:
    /* Check to see if we received any characters from the other radio
     * and if we have send them out the uart and reset indicator timeout.
     * Prioritize on radio received links as that buffer is the most time
     * critical with respect to the UART buffers.
     */
    
    //receive
    if( SMPL_Receive( LinkID, tx, &len ) == SMPL_SUCCESS )
    {
      /* blocking call but should be ok if both ends have same uart buad rate */

      led_tmr = INDICATOR_TIME_OUT;   /* update activity time out */

        if(device_type==DEVICE_TYPE_LINK){//LinkTo
          if(tx[0]==0xfb&&tx[1]==0xfb&&tx[2]==0xfb&&display_vaule==0){
            //two 8bit ->16bit
            display_vaule=1;
            listen_receive_count=tx[4];
            listen_receive_count=listen_receive_count<<8;
            listen_receive_count=listen_receive_count+tx[3];//¥ª±Û¸É0
            unsigned char char_num[10];
            
            
            //test¬O§Aªº16bitªº­È
            //unsigned char test_num[10];
            
            itoa_int16( listen_receive_count,char_num);
            tx_send_wait(&char_num, strlen(char_num));
            MRFI_DelayMs( 10 );
            tx_send_wait( "end\r\n", sizeof("end\r\n") );
            MRFI_DelayMs( 10 );        
            link_count=0;                                  
          }

        }else
        //receive of listen
        if(device_type==DEVICE_TYPE_LISTEN){//Listen
 
          link_receive_count=tx[4];
          link_receive_count=link_receive_count<<8;
          link_receive_count=link_receive_count+tx[3];//¥ª±Û¸É0
          
          if(tx[0]==0xbf&&tx[MAX_APP_PAYLOAD-1]==0x00){
            if(link_receive_count<number-1){              
              listen_count=listen_count+1;
              listen_transmit_on=0;
              listen_count_tmp=listen_count;
                  
            }else if(link_receive_count==(number-1)){
              listen_count=listen_count+1;
              listen_count_tmp=listen_count;
              listen_transmit_on=1;
              listen_count=0;
              
            }else{
              listen_count=0;
              listen_transmit_on=1;
            }
          }
        }
      
    }
    
    //transmit of link
    if(device_type==DEVICE_TYPE_LINK){

      if(link_count < number){             
        
        msg[0]=0xbf;
        msg[MAX_APP_PAYLOAD-1]=0x00;
        
        msg[3]=link_count;//low bit
        msg[4]=link_count>>8;//high bit
        
        SMPL_Send(LinkID, msg, sizeof(msg));
        link_count=link_count+1;
        
      }else if(monitor()==1){
        msg[0]=0xbf;
        msg[MAX_APP_PAYLOAD-1]=0x00;
        
        msg[3]=link_count;//low bit
        msg[4]=link_count>>8;//high bit
        
        SMPL_Send(LinkID, msg, sizeof(msg));
        link_count=link_count+1;
      }
      
      if(link_count == number){
        display_vaule=0;
      }      
    }
    
    //transmit of listen
    if(device_type==DEVICE_TYPE_LISTEN){     
      if(listen_transmit_on==1 && monitor()==1){
        //16bit ->two 8bit
        msg[0]=0xfb;
        msg[1]=0xfb;
        msg[2]=0xfb;
        msg[3]=listen_count_tmp;//low bit
        msg[4]=listen_count_tmp>>8;//high bit

        tx_send_wait("end\r\n", sizeof("end\r\n") );
        MRFI_DelayMs( 10 );        
      
        while(SMPL_Send(LinkID, msg, sizeof(msg))!= SMPL_SUCCESS ){
        }                   
      }
    }
    
    

    /* manage led indicator */
    if( led_tmr != 0 )
    {
      led_tmr--;
      BSP_TURN_ON_LED1( );
    }
    else
      BSP_TURN_OFF_LED1( );
    
    goto main_loop; /* do it again and again and again and ... */
    

}

    int monitor(void){
      if((T1STAT & 0x20) == 0x20){  // µ¥«Ý©w®É®É¶¡¨ì¡]·¸¥X¼Ð»x¬°1¡^
        T1STAT &= ~0x20;              // ²M¹s·¸¥X¼Ð»x
        return 1;
      }else{
        return 0;
      }
    }

    /* itoa:  convert n to characters in s */
    static void itoa_int16(int16_t n,unsigned char s[])
    {
      int16_t i, sign;
 
      if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
      i = 0;
      do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
      } while ((n /= 10) > 0);     /* delete it */
       if (sign < 0)
         s[i++] = '-';
      s[i] = '\0';
      reverse(s);
    }
     /* reverse:  reverse string s in place */
    static void reverse(unsigned char s[])
    {
      int i, j;
      unsigned char c;
 
      for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
      }
    }

    /*Too time-consuming*/
    size_t strlen(const unsigned char * str) {
      const unsigned char *cp =  str;
      while (*cp++ );
      return (cp - str - 1 );
    }
    /*Too time-consuming*/
    size_t strlen_b(const signed char * str) {
      const signed char *cp =  str;
      while (*cp++ );
      return (cp - str - 1 );
    } 