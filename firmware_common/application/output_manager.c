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
#include <output_manager.h>

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
extern volatile bool G_bApplicationNotReady;


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "OutputManager_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type OutputManager_pfStateMachine;               /*!< @brief The state machine function pointer */
static fnCode_type OutputManager_pfPreAction;                  /*< @brief A pre-action before resuming the state */
static BlinkSequenceType OutputManager_pBlinkLed;              /*!< @brief blink sequence of leds  */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */
/*--------------------------------------------------------------------------------------------------------------------*/

void OutputManagerSwitchOutputState(LedOutputStateType eNewState) {
    clearLeds();
    if (OutputManager_pBlinkLed.pSequence != NULL) {
        deleteSequence();
    }

    switch (eNewState) {
        case LED_OUTPUT_NONE:
            OutputManager_pfStateMachine = OutputManagerSM_Idle;
            break;
        case LED_OUTPUT_GREEN:
            LedOn(GREEN);
            OutputManager_pBlinkLed.pSequence = (void *) GREEN;
            OutputManager_pfStateMachine = OutputManagerSM_Idle;
            break;
        case LED_OUTPUT_RED:
            LedOn(RED);
            OutputManager_pBlinkLed.pSequence = (void *) RED;
            OutputManager_pfStateMachine = OutputManagerSM_Idle;
            break;
        case LED_OUTPUT_RGFLASH: {
            LedNameType *pSequence = malloc(sizeof(LedNameType) * 2);
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

void OutputManagerSetAlert(LedOutputAlertType ledAlert, bool hold) {
    if (ledAlert != LED_ALERT_NONE && OutputManager_pfPreAction == NULL) {
        OutputManager_pfPreAction = OutputManagerPA_LedAlert;
        if(hold == TRUE)
        {
            holdState(FALSE);
        }
        LedNameType *sequence;
        switch (ledAlert) {
            case LED_ALERT_ORANGE:
                sequence = malloc(sizeof(LedNameType) * 1);
                *sequence = ORANGE;
                OutputManager_pBlinkLed = (BlinkSequenceType) {.pSequence = sequence, .u8SequenceLength = 1, .u32SwitchingTime = ALERT_DURATION};
                break;
            case LED_ALERT_REDFLASH:
                sequence = malloc(sizeof(LedNameType) * 2);
                sequence[0] = RED;
                sequence[1] = 100;
                OutputManager_pBlinkLed = (BlinkSequenceType) {.pSequence = sequence, .u8SequenceLength = 2, .u32SwitchingTime =
                ALERT_DURATION / 6 / 2, .u8Repetitions = 6};
                break;
            case LED_ALERT_ORANGEFLASH:
                sequence = malloc(sizeof(LedNameType) * 2);
                sequence[0] = ORANGE;
                sequence[1] = 100;
                OutputManager_pBlinkLed = (BlinkSequenceType) {.pSequence = sequence, .u8SequenceLength = 2, .u32SwitchingTime =
                ALERT_DURATION / 6 / 2, .u8Repetitions = 6};
                break;
            default:
                DebugPrintf("Invalid Alert!\n");
        }
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
void OutputManagerInitialize(void) {
    /* If good initialization, set state to Idle */
    if (1) {
        OutputManager_pfStateMachine = OutputManagerSM_Idle;
        OutputManager_pBlinkLed = (BlinkSequenceType) {0};
        DebugPrintf("OutputManager Initialized\n");
    } else {
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
void OutputManagerRunActiveState(void) {
    if (OutputManager_pfPreAction != NULL) {
        OutputManager_pfPreAction();
    }
    if(G_bApplicationNotReady == FALSE) {
        OutputManager_pfStateMachine();
    }
} /* end OutputManagerRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */
/*--------------------------------------------------------------------------------------------------------------------*/

static void clearLeds(void) {
    int i = 0;
    for (LedNameType eLed = WHITE; eLed <= RED; LedOff(eLed++));
}

static void deleteSequence(void) {
    if (OutputManager_pBlinkLed.pSequence != NULL) {
        DebugPrintf("Led Sequence Does Not Exist!\n");
        return;
    }
    free(OutputManager_pBlinkLed.pSequence);
    OutputManager_pBlinkLed.pSequence = NULL;
}

static bool holdState(bool resume) {
    static fnCode_type heldState = OutputManagerSM_Idle;
    static BlinkSequenceType heldSequence;
    static LedNameType prev_led;
    clearLeds();
    if (resume == TRUE) {
        if (heldState != OutputManager_pfStateMachine || heldState == OutputManagerSM_Idle) {
            if ((LedNameType) heldSequence.pSequence >= WHITE && (LedNameType) heldSequence.pSequence <= RED) {
                LedOn((LedNameType) heldSequence.pSequence);
            }
            OutputManager_pfStateMachine = heldState;
            deleteSequence();
            OutputManager_pBlinkLed = heldSequence;
            OutputManager_pBlinkLed.bInitialized = FALSE;
            G_bApplicationNotReady = FALSE;
        }
    } else {
        G_bApplicationNotReady = TRUE;
        heldState = OutputManager_pfStateMachine;
        heldSequence = OutputManager_pBlinkLed;
        OutputManager_pfStateMachine = OutputManagerSM_Hold;
    }
}

static void alertSound() {

    static short i;

    short temp = sequenceTracker(FALSE);

    if (temp >= 0 && temp < 100) {
        i = temp;
        PWMAudioSetFrequency(BUZZER1, (u16) (RED-OutputManager_pBlinkLed.pSequence[i]) * 50 + 100);
        PWMAudioOn(BUZZER1);
    }
    if (i == -2) {
        i = 0;
        PWMAudioOff(BUZZER1);
    }
}

static short sequenceTracker(bool bIncrement) {
    static u32 u32SequenceTimer;
    static short s8SequenceCursor;

    //detect new sequence
    if (OutputManager_pBlinkLed.bInitialized == FALSE) {
        u32SequenceTimer = G_u32SystemTime1ms;
        s8SequenceCursor = 0;

//        clearLeds();
//        LedOn(OutputManager_pBlinkLed.pSequence[s8SequenceCursor]);
        OutputManager_pBlinkLed.bInitialized = TRUE;
    }

    //run sequence
    if (G_u32SystemTime1ms - u32SequenceTimer >= OutputManager_pBlinkLed.u32SwitchingTime) {
//        if(OutputManager_pBlinkLed.pSequence[s8SequenceCursor] != 100)
//        {
//            LedOff(OutputManager_pBlinkLed.pSequence[s8SequenceCursor++]);
//        }
//        else
//        {
//            s8SequenceCursor++;
//        }
        //check for end of sequence
        if (bIncrement == TRUE) {
            s8SequenceCursor++;
            if (s8SequenceCursor == OutputManager_pBlinkLed.u8SequenceLength) {
                //check for end of repetitions
                if (OutputManager_pBlinkLed.u8Repetitions == 0) {
                    OutputManagerSwitchOutputState(LED_OUTPUT_NONE);
                    u32SequenceTimer = G_u32SystemTime1ms;
                    s8SequenceCursor = -2;
                    return -2;
                } else if (OutputManager_pBlinkLed.u8Repetitions != CONTINUOUS_SEQUENCE) {
                    OutputManager_pBlinkLed.u8Repetitions--;
                }

                s8SequenceCursor = 0;
            }
        }
        u32SequenceTimer = G_u32SystemTime1ms;
        return s8SequenceCursor;
    }
    if(bIncrement == FALSE && G_u32SystemTime1ms - u32SequenceTimer == 0)
    {
        return s8SequenceCursor;
    }
    return -1;
}

static void OutputManagerPA_LedAlert(void) {
    //stop state interfering with led alert pattern
    if (OutputManager_pfStateMachine != OutputManagerSM_Hold) {
        //holds current state
        holdState(FALSE);
    }
    OutputManagerSM_LedSequence();
    alertSound();
    if (OutputManager_pfStateMachine == OutputManagerSM_Idle) {
        holdState(TRUE);
        OutputManager_pfPreAction = NULL;
    }
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void OutputManagerSM_Idle(void) {

} /* end OutputManagerSM_Idle() */

static void OutputManagerSM_LedSequence(void) {
    static short i;

    short temp = sequenceTracker(TRUE);

    if (temp >= WHITE && temp <= RED) {
        if (i >= WHITE && i <= RED) {
            LedOff(OutputManager_pBlinkLed.pSequence[i]);
        }
        i = temp;
        LedOn(OutputManager_pBlinkLed.pSequence[i]);
    }
    if (i == -2) {
        i = 0;
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void OutputManagerSM_Error(void) {

} /* end OutputManagerSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
