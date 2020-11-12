#include "stdint.h"


#define SWM_ChangeEdgeDetection() { \
 									\
 	TCCR1B ^= (1u << ICES1);        \
                                    \
 	TIFR1 |= (1u << ICF1);          \
                                    \
}        

typedef enum {
  
 SWM_nenInvalid = 0,
  
 SWM_nenHigh = 1,
  
 SWM_nenLow  = 2
  
}SWM__tenPulseLevel;


typedef union {
  
 uint16_t u16Ticks;
 
 struct {
   
    uint8_t u8Low;
   
    uint8_t u8High;
    
 }byte;
  
}SWM__tstTimeStamp;


static volatile SWM__tenPulseLevel SWM__enCurrentPulse;
static volatile SWM__tenPulseLevel SWM__enPreviousPulse = SWM_nenInvalid;;
static volatile SWM__tenPulseLevel SWM__enPeriodPulse;

static volatile uint16_t 		   SWM__u16OverflowCounter;

static volatile SWM__tstTimeStamp  SWM__stCapture;

static volatile uint32_t 		   SWM__u32CurrentCapture;
static volatile uint32_t           SWM__u32PreviousCapture;

static volatile SWM__tstTimeStamp  SWM__stPeriodCapture;


static volatile uint32_t u32Flag;

void 
setup() {
  
  
  
  SWM__enCurrentPulse = SWM_nenLow;
  
  cli();
  
  TCCR1A = 0;
  
  /* No prescaler, falling edge capture*/
  TCCR1B = (1u << CS10);
  
  TCCR1C = 0;
  
  /* Enable overflow and ICP interrupt */
  TIMSK1 = ((1u << ICIE1) | (1u << TOIE1));
  
  TIFR1 = ((1u << ICF1) | (1u << TOV1)); 
  
  TCNT1  = 0x00;
  
  pinMode(8, INPUT);
  
  /* No floating */
  digitalWrite(8, LOW);
  
  sei();
   
  Serial.begin(9600);
  
  
}

void 
loop() {
  
  if(u32Flag == 1)
    Serial.println("High");
  else if(u32Flag == 2)
    Serial.println("Low");
    
   Serial.println(SWM__enCurrentPulse);
}


ISR(TIMER1_OVF_vect) {

  ++SWM__u16OverflowCounter;
  
  TCNT1 = 0x00;
}

ISR(TIMER1_CAPT_vect) {
  
  /* Capture the ticks */
  SWM__stCapture.byte.u8Low  = ICR1L;
  SWM__stCapture.byte.u8High = ICR1H;
  
  if(SWM__u16OverflowCounter > 0) {
    
  	SWM__u32CurrentCapture = (((uint32_t)(SWM__u16OverflowCounter << 16)) + SWM__stCapture.u16Ticks);  

    /* Reset the overflow counter as we included it in the calculation */ 
    SWM__u16OverflowCounter = 0;
  
   }
  
  
  /* Check if It's the first capture */
  if(SWM__enPreviousPulse != 0) {
  
    switch(SWM__enCurrentPulse) {
      
       case SWM_nenLow:

        /* High time */
        if(SWM__enPreviousPulse == SWM_nenHigh) {

          /* Consider overflow */
          if(SWM__u32CurrentCapture > SWM__u32PreviousCapture) {
			
            /* Call the high time callback */
           
          } else {

            /* Call the high time callback */
            
          } 
          
          u32Flag = 1;
          
          SWM__enCurrentPulse = SWM_nenHigh;

        }
      
        break;

       case SWM_nenHigh:
       
      	/* High time */
        if(SWM__enPreviousPulse == SWM_nenLow) {

          /* Consider overflow */
          if(SWM__u32CurrentCapture > SWM__u32PreviousCapture) {

            /* Call the low time callback */

          } else {

            /* Call the low time callback */

          } 
          
          u32Flag = 2;
          
		   SWM__enCurrentPulse = SWM_nenLow;
          
        }
      
			
        break;
    }
  
  }
  
  /* Save the current time stamp */
  SWM__u32PreviousCapture = SWM__u32CurrentCapture;
  
  SWM__enPreviousPulse = (SWM__tenPulseLevel)(SWM__enCurrentPulse ^ 0x00000003);
  
  /* Change the edge */
  SWM_ChangeEdgeDetection();
  
  
}

