/*!*********************************************************************************************************************
@file output_manager.h                                                                
@brief Header file for output_manager

**********************************************************************************************************************/

#ifndef __OUTPUT_MANAGER_H
#define __OUTPUT_MANAGER_H
#include <configuration.h>
#include <eief1-pcb-01.h>

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/

/*!
@enum LedOutputStateType
*/
typedef enum {LED_OUTPUT_NONE = 0, LED_OUTPUT_RED, LED_OUTPUT_GREEN, LED_OUTPUT_RGFLASH}LedOutputStateType;

/*!
@enum LedOutputAlertType
 */
 typedef enum {LED_ALERT_NONE = 0, LED_ALERT_ORANGE, LED_ALERT_REDFLASH, LED_ALERT_ORANGEFLASH}LedOutputAlertType;

 /*!
  @struct BlinkSequenceType
  */
  typedef struct{
      LedNameType* pSequence;
      u8 u8SequenceLength, u8Repetitions;
      u32 u32SwitchingTime;
      bool bInitialized;
  } BlinkSequenceType;

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!
@fn OutputManagerSwitchOutputState(LedOutputStateType pNewState)
*/
 void OutputManagerSwitchOutputState(LedOutputStateType pNewState);

/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void OutputManagerInitialize(void);
void OutputManagerRunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


static void clearLeds(void);
static void deleteSequence(void);

/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void OutputManagerSM_Idle(void);
static void OutputManagerSM_Error(void);

static void OutputManagerSM_LedSequence(void);



/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
#define CONTINUOUS_SEQUENCE (u8) 255

#endif /* __OUTPUT_MANAGER_H */
/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
