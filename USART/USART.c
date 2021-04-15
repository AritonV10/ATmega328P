/****************************************************************************************
 * 
 *   Author      : Ariton Viorel
 *   Created     : 3/2/2021
 *   Description : Implementation file
 * 
 */

/*****************************************************************************************/
/********************************** Includes *********************************************/

#include "USART.h"

#include "USART_Def.h"


/*****************************************************************************************/
/******************************* Macros & Defines ****************************************/

#if defined USART_nParity_Cfg != 0
  #define USART__nErrorMask ( (1u << USART__nUPE0) | (1u << USART__nDOR0) | (1u << USART__nFE0) ) 
#else
  #define USART__nErrorMask ( (1u << USART__nDOR0) | (1u << USART__nFE0) ) 
#endif

/* 16MHz */
#define USART__FCLK 16e6


/* 32 bytes */
#define USART__nRxBufferSize 0x20u
#define USART__nTxBufferSize 0x20u


#define USART__nMinBits 0x05u
#define USART__nMaxBits 0x09u

#define USART__nMinStopBits 0x01u
#define USART__nMaxStopBits 0x02u

#define USART__vDisableISR() (USART__SREG &= ~(1u << 7))
#define USART__vEnableISR() (USART__SREG |= (1u << 7))

#define USART__u32AsyncNormalBaudRate(Baud) (((uint32_t)((USART__FCLK/USART__nAsyncNormalSampling) * 1/Baud)) - 1u)




/*****************************************************************************************/
/**************************** Private Function Declarations ******************************/


/*****************************************************************************************/
/**************************** Private Declarations ***************************************/



#if defined USART_nDataBits_Cfg <= USART_nDataBits8
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


/*****************************************************************************************/
/**************************** Public Function Definitions ********************************/



#if defined USART_AsyncISR_Cfg == 1
  
  ISR(USART0_RX_vect) {
    
    uint8_t u8Status;
    uint8_t u8Byte;
    
    u8Status = USART__UCSR0A;
    u8Byte   = USART__UDR0;
        
     /* Check for errors */
     if(u8Status & USART__nErrorMask)
        return;
     
     USART__au8RxBuffer[USART__u8RxIdxISR];
     USART__u8RxIdxISR = ((USART__u8RxIdxISR + 1) & (USART__nRxBufferSize - 1));
     
  }
#endif
#if defined USART_Async_Cfg == 1

  void
  USART_vInit(void) {

    USART__vDisableISR();
    
    USART__UCSR0C = 0;
    USART__UCSR0B = 0;

    /* Configure the baud rate */
    USART__UBRR0L = (uint8_t)(USART__u32AsyncNormalBaudRate(USART_nBaudRate_Cfg) & 0x000000FFu);
    USART__UBRR0H = (uint8_t)(USART__u32AsyncNormalBaudRate(USART_nBaudRate_Cfg) >> 8);
  
    /* Configure the data bits (2:3)-3 */
    USART__UCSR0C |= ((USART_nDataBits_Cfg & 0x03u) << USART__nUCSZ00);
    USART__UCSR0B |= ((USART_nDataBits_Cfg & 0x04u));
  
    /* Configure transmission (4:3) */
    USART__UCSR0B |= (USART_nTranmission_Cfg << USART__nTranmitterEnable);
  
    /* Configure parity (5:4) */
    USART__UCSR0C |= (USART_nParity_Cfg << USART__nUPM00);
  
    /* Configure stop bits (3) */
    USART__UCSR0C |= (USART_nStopBits_Cfg << USART__nUSBS0);

    #if defined USART_AsyncISR_Cfg == 1
      USART__UCSR0B = (1u << USART__nRXCIE0);
    #endif

    USART__vEnableISR();
  
  }
  
  #if defined USART_nDataBits_Cfg <= USART_nDataBits8
  
    uint8_t
    USART_u8Read(void) {
      uint8_t u8Byte;
      
      #if defined USART_AsyncISR_Cfg == 1
         
         /* Waiting for data */
         while( USART__u8RxIdx == USART__u8RxIdxISR )
          ;
          /* Get the byte from the Rx buffer */
         u8Byte = USART__au8RxBuffer[USART__u8RxIdx];

         /* Wraparound*/
         USART__u8RxIdx = ((USART__u8RxIdx + 1) & (USART__nRxBufferSize - 1));

         return(u8Byte);
         
      #else
      
        uint8_t u8Status;
        
        /* Busy wait until data is received */
        while (!(USART__UCSR0A & (1u << USART__nRXC0)))
          ;
        
        u8Status = USART__UCSR0A;
        u8Byte   = USART__UDR0;
        
        /* Check for errors */
        if(u8Status & USART__nErrorMask)
          return(-1);
        
        return (u8Byte);
      #endif
      
    }
    
    void
    USART_vSend(uint8_t u8Byte) {
      
      /* Busy wait until we can place the byte into the buffer */
      while (!(USART__UCSR0A & (1u << USART__nUDRE0)))
        ;
    
      USART__UDR0 = u8Byte;
    }
  #else /* 9th bit used */
    
    uint16_t
    USART_u16Read(void) {
      
      uint8_t u8Status;
      uint8_t u8Upper;
      uint8_t u8Lower;
          
      /* Busy wait until data is received */
      while (!(USART__UCSR0A & (1u << USART__nRXC0)))
        ;
      
      u8Status = USART__UCSR0A;

      /* Save the 9th bit and the lower byte */
      u8Upper = USART__UCSR0B;
      u8Lower = USART_UDR0;
      
      /* Check for errors */
      if(u8Status & USART__nErrorMask)
        return(-1)

      u8Upper = (USART__UCSR0B & USART__nRXB90);
      
      return (
    }
    
    void
    USART_vSend(uint16_t u16Byte) {
      
      /* Busy wait until we can place the byte into the buffer */
      while (!(USART__UCSR0A & (1u << USART__nUDRE0)))
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
    
    for(; *puStr != 0; ++puStr)
      USART_vSend(*puStr);
    
  }

  void
  USART_vSendU32(uint32_t u32Data) {
    
  }
  
#endif
