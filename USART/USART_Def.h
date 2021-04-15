#ifndef USART_DEF__H__GUARD
#define USART_DEF__H__GUARD

  #include "stdint.h"

  #define USART__SREG   *(volatile uint8_t *)0x5Fu
  /* Baud rate high/low */
  #define USART__UBRR0H *(volatile uint8_t *)0xC5u
  #define USART__UBRR0L *(volatile uint8_t *)0xC4u

  /* Data register */
  #define USART__UDR0   *(volatile uint8_t *)0xC6u

  /* Control registers */
  #define USART__UCSR0C *(volatile uint8_t *)0xC2u
  #define USART__UCSR0B *(volatile uint8_t *)0xC1u
  #define USART__UCSR0A *(volatile uint8_t *)0xC0u


  /* Async normal mode - 16 samples */
  #define USART__nAsyncNormalSampling 0x00000010u
  /* Async double speed - 8 samples */
  #define USART__nAsyncDoubleSampling 0x00000008u

  /* Receive Complete */
  #define USART__nRXC0 0x07u
  /* Transmit Complete */
  #define USART__nTXC0 0x06u
  /* Data Register Empty */
  #define USART__nUDRE0 0x05u
  /* Error bits */
  #define USART__nFE0  0x04u
  #define USART__nDOR0 0x03u
  #define USART__nUPE0 0x02u
  /* Double Tranmission Speed */
  #define USART__nU2X0 0x01
  /* Multi-processor Communication Mode */
  #define USART__nMPCM0 0x00u

  /* Rx Complete Interrupt Enable */
  #define USART__nRXCIE0 0x07u
  /* Tx Complete Interrupt Enable */
  #define USART__nTXCIE0 0x06u
  /* Data Register Empty Interrupt Enable */
  #define USART__nUDRIE0 0x05u
  /* Receiver & Transmitter enable */
  #define USART__nRxEnable 0x04u
  #define USART__nTxEnable 0x03u
  #define USART__nTranmitterEnable 0x03u
  /* Character Size */
  #define USART__nUCSZ02 0x02u
  /* 9th data bit */
  #define USART__nRXB90  0x01u
  #define USART__nTXB90  0x00u

  /* Operation Mode */
  #define USART__nUMSEL01 0x07u
  #define USART__nUMSEL00 0x06u
  /* Parity Mode */
  #define USART__nUPM01 0x05u
  #define USART__nUPM00 0x04u
  /* Stop Bit Select */
  #define USART__nUSBS0 0x03u
  /* Data Bits */
  #define USART__nUCSZ01 0x02u
  #define USART__nUCSZ00 0x01u
  /* Clock Polarity */
  #define USART__nUCPOL0 0x00u

#endif
