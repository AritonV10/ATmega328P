/****************************************************************************************
 * 
 *   Author      : Ariton Viorel
 *   Created     : 4/16/2021
 *   Description : Source file
 *   
 */

/*****************************************************************************************/
/********************************** Includes *********************************************/


#include <avr/interrupt.h>

#include "USART.h"
#include "USART_Def.h"


#ifndef USART_nBaudRate_Cfg
  #error "[USART_Err]: 'USART_nBaudRate_Cfg' not defined"
#endif
#if USART_nBaudRate_Cfg < 0
  #error "[USART_Err]: 'USART_nBaudRate_Cfg' is negative"
#endif 
#ifndef USART_nDataBits_Cfg
  #error "[USART_Err]: 'USART_nDataBits_Cfg' not defined"
#endif
#if USART_nDataBits_Cfg != USART_nDataBits5 && \
    USART_nDataBits_Cfg != USART_nDataBits6 && \
    USART_nDataBits_Cfg != USART_nDataBits7 && \
    USART_nDataBits_Cfg != USART_nDataBits8 && \
    USART_nDataBits_Cfg != USART_nDataBits9

  #error "[USART_Err]: Invalid data bits"
  
#endif

/*****************************************************************************************/
/******************************* Macros & Defines ****************************************/

#if USART_nParity_Cfg != 0
  #define USART__nErrorMask ( (1u << USART__nUPE0) | (1u << USART__nDOR0) | (1u << USART__nFE0) ) 
#else
  #define USART__nErrorMask ( (1u << USART__nDOR0) | (1u << USART__nFE0) ) 
#endif

/* 16MHz */
#define USART__FCLK 16e6


/* 32 bytes */
#define USART__nRxBufferSize 0x20u
#define USART__nTxBufferSize 0x20u

#define USART__nRxRingBuf (USART__nRxBufferSize - 1u)
#define USART__nTxRingBuf (USART__nTxBufferSize - 1u)

#define USART__vDisableISR() (USART__SREG &= ~(1u << 7))
#define USART__vEnableISR() (USART__SREG |= (1u << 7))

#define USART__u32AsyncNormalBaudRate(Baud) (((uint32_t)((USART__FCLK/USART__nAsyncNormalSampling) * 1/Baud)) - 1u)

/*****************************************************************************************/
/**************************** Private Function Declarations ******************************/


/*****************************************************************************************/
/**************************** Private Declarations ***************************************/

#if USART_AsyncISR_Cfg == 1
  #if USART_nDataBits_Cfg <= USART_nDataBits8
    static volatile uint8_t USART__au8RxBuffer[USART__nRxBufferSize];
    static volatile uint8_t USART__au8TxBuffer[USART__nTxBufferSize];
  #else
    static volatile uint16_t USART__au16RxBuffer[USART__nRxBufferSize];
    static volatile uint16_t USART__au16TxBuffer[USART__nTxBufferSize];
  #endif
  
  static volatile uint8_t USART__u8RxIdx;
  static volatile uint8_t USART__u8RxIdxISR;
  
  static volatile uint8_t USART__u8TxIdx;
  static volatile uint8_t USART__u8TxIdxISR;
#endif

/*****************************************************************************************/
/**************************** Public Function Definitions ********************************/

#if USART_AsyncISR_Cfg == 1
  
  ISR(USART_RX_vect) {
    
    uint8_t u8Status;
    uint8_t u8Byte;
    
    u8Status = USART__UCSR0A;
    u8Byte   = USART__UDR0;
    
    /* Check for errors */
    if(u8Status & USART__nErrorMask) {
      return;
    }
     
    USART__au8RxBuffer[USART__u8RxIdxISR] = u8Byte;
    USART__u8RxIdxISR = ((USART__u8RxIdxISR + 1u) & USART__nRxRingBuf);
     
  }

  ISR(USART_UDRE_vect) {
    
    /* No new data in the buffer to be sent */
    if(USART__u8TxIdx == USART__u8TxIdxISR)
      return;

    
    USART__UDR0 = USART__au8TxBuffer[USART__u8TxIdxISR];
  
    USART__u8TxIdxISR = ((USART__u8TxIdxISR + 1u) & USART__nTxRingBuf);
     
  }
#endif
#if USART_Async_Cfg == 1

  void
  USART_vInit(void) {

    USART__vDisableISR();
    
    USART__UCSR0C = 0;
    USART__UCSR0B = 0;

    /* Configure the baud rate */
    USART__UBRR0L = (USART__u32AsyncNormalBaudRate(USART_nBaudRate_Cfg) & 0x000000FFu);
    USART__UBRR0H = (USART__u32AsyncNormalBaudRate(USART_nBaudRate_Cfg) >> 8);
  
    /* Configure the data bits (2:3)-3 */
    USART__UCSR0C |= ((USART_nDataBits_Cfg & 0x03u) << USART__nUCSZ00);
    USART__UCSR0B |= ((USART_nDataBits_Cfg & 0x04u));
  
    /* Configure transmission (4:3) */
    USART__UCSR0B |= (USART_nTranmission_Cfg << USART__nTranmitterEnable);
  
    /* Configure parity (5:4) */
    USART__UCSR0C |= (USART_nParity_Cfg << USART__nUPM00);
  
    /* Configure stop bits (3) */
    USART__UCSR0C |= (USART_nStopBits_Cfg << USART__nUSBS0);

    #if USART_AsyncISR_Cfg == 1
      USART__UCSR0B |= (USART_nTranmission_Cfg << USART__nISREnable);
      USART__UCSR0B |= ((USART_nTranmission_Cfg & 0x01) << USART__nUDRIE0);
    #endif

    USART__vEnableISR();
  
  }
  
  #if USART_nDataBits_Cfg <= USART_nDataBits8
  
    uint8_t
    USART_u8Read(void) {
      uint8_t u8Byte;
      
      #if USART_AsyncISR_Cfg == 1
         
        /* Check if there is no new data received */
        if( USART__u8RxIdx == USART__u8RxIdxISR )
          return(USART_nRxError);
         
        /* Get the byte from the Rx buffer */
        u8Byte = USART__au8RxBuffer[USART__u8RxIdx];

        /* Wraparound*/
        USART__u8RxIdx = ((USART__u8RxIdx + 1u) & USART__nRxRingBuf);

        return(u8Byte);
         
      #else
      
        uint8_t u8Status;
        
        /* Busy wait until data is received */
        for(; !(USART__UCSR0A & (1u << USART__nRXC0)); )
          ;
        
        u8Status = USART__UCSR0A;
        u8Byte   = USART__UDR0;
        
        /* Check for errors */
        if(u8Status & USART__nErrorMask)
          return(USART_nRxError);
        
        return(u8Byte);
      #endif
      
    }
    
    int8_t
    USART_vSend(uint8_t u8Byte) {
      
      #if USART_AsyncISR_Cfg == 1 /* ISR enabled */
         
        #if USART_AsyncISR_BufferError_Cfg == 1
          /* Check to see if the buffer is full */
          if( ((USART__u8TxIdx + 1u) & USART__nTxRingBuf) == USART__u8TxIdxISR ) {
            return(USART_nBufferFullError);
          }    
        #endif
         
        USART__au8TxBuffer[USART__u8TxIdx] = u8Byte;
         
        /* Inc idx */
        USART__u8TxIdx = ((USART__u8TxIdx + 1u) & USART__nTxRingBuf);
         
      #else
        /* Busy wait until we can place the byte into the buffer */
        for(;!(USART__UCSR0A & (1u << USART__nUDRE0));)
          ;
         
        USART__UDR0 = u8Byte;
      #endif

      return(USART_nTxOk);
    }
  #else /* 9th bit used */
    
    uint16_t
    USART_u16Read(void) {
      
      uint8_t u8Status;
      uint8_t u8Upper;
      uint8_t u8Lower;
          
      /* Busy wait until data is received */
      for(;!(USART__UCSR0A & (1u << USART__nRXC0));)
        ;
      
      u8Status = USART__UCSR0A;

      /* Save the 9th bit and the lower byte */
      u8Upper = USART__UCSR0B;
      u8Lower = USART_UDR0;
      
      /* Check for errors */
      if(u8Status & USART__nErrorMask)
        return(USART_nRxError)

      u8Upper = (USART__UCSR0B & USART__nRXB90);
      
      return((u8Upper << 8) | u8Lower);
    }
    
    void
    USART_vSend(uint16_t u16Byte) {
      
      /* Busy wait until we can place the byte into the buffer */
      for(;!(USART__UCSR0A & (1u << USART__nUDRE0));)
        ;

      /* Clear  & set the 9th bit */
      USART__UCSR0B &= ~(1u << USART__nTXB90);
      USART__UCSR0B |= (((u16Byte & 0x0100) > 0) << USART__nTXB90);

      /* Set the lower byte */
      USART__UDR0 = (u16Byte & 0x00FF);
      
    }
  #endif

  void
  USART_vSendString(uint8_t *puStr) {
    
    int8_t i8Err;
    
    for(; *puStr != 0;) {
      i8Err = USART_vSend(*puStr);
      puStr += (i8Err == USART_nTxOk);
    }
  }

  void
  USART_vSendU32(uint32_t u32Data) {
    
  }
  
#endif
