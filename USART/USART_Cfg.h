#ifndef USART_CFG__H__GUARD
#define USART_CFG__H__GUARD

  #include "USART_Def.h"  


  /*****************************************************************************************/
  /********************************* USART Configuration ***********************************/
 
  #define USART_nBaudRate_Cfg 9600
  #define USART_nDataBits_Cfg USART_nDataBits8
  /*
   * 1: Tx
   * 2: Rx
   * 3: RxTx
   */
  #define USART_nTranmission_Cfg 3
  
  /* 
   * 0: Disabled
   * 2: Even
   * 3: Odd
   */
  #define USART_nParity_Cfg 0
  
  /*
   * 0: 1 SB
   * 1: 2 SB
   */
  #define USART_nStopBits_Cfg 1
  
  
  #define USART_Async_Cfg                1
  #define USART_AsyncNormalMode_Cfg      1
  #define USART_AsyncDoubleMode_Cfg      0

  /*
   * ISR Enabler
   */
  #define USART_AsyncISR_Cfg             1

  /*
   * 0: Data overwrite
   * 1: No data overwrite
   */
  #define USART_AsyncISR_BufferError_Cfg 1
  
  /* Data bits configurations */
  #define USART_nDataBits5 0x00u
  #define USART_nDataBits6 0x01u
  #define USART_nDataBits7 0x02u
  #define USART_nDataBits8 0x03u
  #define USART_nDataBits9 0x07u

#endif
