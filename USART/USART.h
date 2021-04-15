#ifndef USART__H__GUARD
#define USART__H__GUARD
  
  #include "USART_Cfg.h"
  #include "stdint.h"
  
  #define USART_nOk             1
  #define USART_nParityError   -1
  #define USART_nBaudRateError -2
  #define USART_nStopBitsError -3
  
  
  extern void
  USART_vInit(void);
  
  
  #if defined USART_nDataBits_Cfg <= USART_nDataBits8
  
    extern uint8_t
    USART_u8Read(void);
    
    extern void
    USART_vSend(uint8_t);
  
  #else
    
    extern uint16_t
    USART_u16Read(void)

    extern void
    USART_vSend(uint16_t);

  #endif
  
#endif
