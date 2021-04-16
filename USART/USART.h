#ifndef USART__H__GUARD
#define USART__H__GUARD
  
  #include "USART_Cfg.h"
  #include "stdint.h"
 
  #define USART_nRxError          0xFFu

  #define USART_nTxOk             1
  #define USART_nBufferFullError -1
  
  extern void
  USART_vInit(void);
  
  
  #if USART_nDataBits_Cfg <= USART_nDataBits8

    /*
    * Function: USART_u8Read 
    * --------------------
    * Reads a byte from serial
    *    
    *
    *  In: void
    *
    *  returns: byte received
    *           
    */
    extern uint8_t
    USART_u8Read(void);
    
    /*
    * Function: USART_vSend 
    * --------------------
    * Sends a byte
    *    
    *
    *  In: the byte to be sent
    *
    *  returns: void
    *           
    */
    extern int8_t
    USART_vSend(uint8_t);
  
  #else
  
    /*
    * Function: USART_u16Read 
    * --------------------
    * Reads 9 bits from serial
    *    
    *
    *  In: void
    *
    *  returns: the bits received
    *           
    */
    extern uint16_t
    USART_u16Read(void)

    /*
    * Function: USART_vSend 
    * --------------------
    * Sends 9 bits
    *    
    *
    *  In: the 9 bits to be sent
    *
    *  returns: void
    *           
    */
    extern void
    USART_vSend(uint16_t);
  #endif

  extern void
  USART_vSendString(uint8_t *);

  extern void
  USART_vSendU32(uint32_t);
  
#endif
