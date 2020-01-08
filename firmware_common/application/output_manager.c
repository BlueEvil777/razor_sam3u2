/*!*********************************************************************************************************************
@file output_manager.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- CONTINUOUS_SEQUENCE

TYPES
- NONE

PUBLIC FUNCTIONS
- void OutputManagerSwitchOutputState(LedOutputStateType eNewState)

PROTECTED FUNCTIONS
- void OutputManagerInitialize(void)
- void OutputManagerRunActiveState(void)


**********************************************************************************************************************/

#include <configuration.h>

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>OutputManager"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32OutputManagerFlags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "OutputManager_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type OutputManager_pfStateMachine;               /*!< @brief The state machine function pointer */
static BlinkSequenceType OutputManager_pBlinkLed;              /*!< @brief blink sequence of leds  */
static LedOutputAlertType OutputManager_pLedActiveAlert;       /*!< @brief active alert */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

void OutputManagerSwitchOutputState(LedOutputStateType eNewState)
{
    clearLeds();
    if(OutputManager_pBlinkLed.pSequence != NULL)
		{
			deleteSequence();
		}
		
    switch(eNewState)
    {
        case LED_OUTPUT_NONE:
            OutputManager_pfStateMachine = OutputManagerSM_Idle;
            break;
        case LED_OUTPUT_GREEN:
            LedOn(GREEN);
            OutputManager_pfStateMachine = OutputManagerSM_Idle;
            break;
        case LED_OUTPUT_RED:
            LedOn(RED);
            OutputManager_pfStateMachine = OutputManagerSM_Idle;
            break;
        case LED_OUTPUT_RGFLASH:
            {
                LedNameType* pSequence = malloc(sizeof(LedNameType) * 2);
                pSequence[0] = RED;
                pSequence[1] = GREEN;
            	OutputManager_pBlinkLed = (BlinkSequenceType) {.u8SequenceLength = 2, .u32SwitchingTime = 150, .pSequence = pSequence, .u8Repetitions = CONTINUOUS_SEQUENCE};
                OutputManager_pfStateMachine = OutputManagerSM_LedSequence;
            	break;
            }
        default:
            //DebugPrintf("No Output State!");
            exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void OutputManagerInitialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void OutputManagerInitialize(void)
{
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    OutputManager_pfStateMachine = OutputManagerSM_Idle;
    OutputManager_pBlinkLed = (BlinkSequenceType) {0};
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    OutputManager_pfStateMachine = OutputManagerSM_Error;
  }

} /* end OutputManagerInitialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void OutputManagerRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void OutputManagerRunActiveState(void)
{
  OutputManager_pfStateMachine();

} /* end OutputManagerRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

static void clearLeds(void)
{
		int i = 0;
    for(LedNameType eLed = WHITE; eLed <= RED; LedOff(eLed++));
}

static void deleteSequence(void)
{
	if(OutputManager_pBlinkLed.pSequence != NULL)
	{
		DebugPrintf("Led Sequence Does Not Exist!\n");
		return;
	}
	free(OutputManager_pBlinkLed.pSequence);
	OutputManager_pBlinkLed.pSequence = NULL;
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void OutputManagerSM_Idle(void)
{
    
} /* end OutputManagerSM_Idle() */

 static void OutputManagerSM_LedSequence(void)
 {
    static u32 u32SequenceTimer;
    static u8 u8SequenceCursor;

    //detect new sequence
    if(OutputManager_pBlinkLed.bInitialized == FALSE)
    {
        u32SequenceTimer = G_u32SystemTime1ms;
        clearLeds();
        LedOn(OutputManager_pBlinkLed.pSequence[u8SequenceCursor]);
        OutputManager_pBlinkLed.bInitialized = TRUE;
    }

    //run sequence
    if(G_u32SystemTime1ms - u32SequenceTimer >= OutputManager_pBlinkLed.u32SwitchingTime)
    {
        LedOff(OutputManager_pBlinkLed.pSequence[u8SequenceCursor++]);
        //check for end of sequence
        if(u8SequenceCursor == OutputManager_pBlinkLed.u8SequenceLength)
        {
            //check for end of repetitions
            if(OutputManager_pBlinkLed.u8Repetitions == 0)
            {
                OutputManagerSwitchOutputState(LED_OUTPUT_NONE);
                return;
            }
            else if(OutputManager_pBlinkLed.u8Repetitions != CONTINUOUS_SEQUENCE)
            {
                OutputManager_pBlinkLed.u8Repetitions--;
            }

            u8SequenceCursor = 0;
        }
        LedOn(OutputManager_pBlinkLed.pSequence[u8SequenceCursor]);
        u32SequenceTimer = G_u32SystemTime1ms;
    }
 }

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void OutputManagerSM_Error(void)          
{
  
} /* end OutputManagerSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
