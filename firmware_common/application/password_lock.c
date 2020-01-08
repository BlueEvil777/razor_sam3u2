/*!*********************************************************************************************************************
@file password_lock.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.
------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- ButtonNameType P_Ary_DEFAULT_PASSWORD

TYPES
- PasswordStateType
- ButtonInputBufferType

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void PasswordLockInitialize(void)
- void PasswordLockRunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>PasswordLock"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32PasswordLockFlags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "PasswordLock_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type PasswordLock_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 PasswordLock_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void PasswordLockInitialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void PasswordLockInitialize(void)
{
	/* If good initialization, set state to Idle */
	if( 0 )
	{
		PasswordLock_pfStateMachine = PasswordLockSM_Idle;
	}
	else
	{
		/* The task isn't properly initialized, so shut it down and don't run */
		PasswordLock_pfStateMachine = PasswordLockSM_Error;
	}
	
} /* end PasswordLockInitialize() */


/*!----------------------------------------------------------------------------------------------------------------------
@fn void PasswordLockRunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void PasswordLockRunActiveState(void)
{
	PasswordLock_pfStateMachine();
	
} /* end PasswordLockRunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!

@fn bool waitingForBtn(ButtonNameType)

@brief Returns the boolean state of a given button. Agknowledges it if TRUE.

Requires: A valid ButtonNameType variable.
Promises: Return if a given button was pressed. Agknowledges the button if true.
*/
bool waitingForBtn(ButtonNameType btn)
{
	if(WasButtonPressed(btn) == TRUE)
	{
		ButtonAcknowledge(btn);
		return TRUE;
	}
	return FALSE;
}

/*!
@fn bool updateBtnInputBuffer(ButtonInputBufferType)

@brief updates the buffer of inputted pins.

Adds buttons to input buffer if a password button is pressed. Returns if a button is added.
*/
bool updateBtnInputBuffer(ButtonInputBufferType* pBtnInputBuffer)
{
	for(ButtonNameType btn = BUTTON0; btn <= BUTTON2; btn++)
	{
		if(waitingForBtn(btn))
		{
			pBtnInputBuffer->p_arryBuffer[pBtnInputBuffer->u8Size] = btn;
			pBtnInputBuffer->u8Size++;
			return TRUE;
		}
	}
	return FALSE;
}

/*!
@fn short checkPassword(ButtonInputBufferType*, const ButtonInputBufferType*)

@brief ...
*/
short checkPassword(ButtonInputBufferType* input, const ButtonInputBufferType* password)
{
	if(waitingForBtn(BUTTON3))
	{
		u8 u8Size = input->u8Size;
		input->u8Size = 0;
		if(u8Size == password->u8Size)
		{
			for(int i = 0; i < u8Size; i++)
			{
				if(input->p_arryBuffer[i] != password->p_arryBuffer[i])
				{
					return -1;
				}
			}
			return 1;
		}
		return -1;
	}
	return 0;
}

/*!
@fn void clearLeds(void)
*/
static void clearLeds(void)
{
	for(LedNameType led = WHITE; led <= RED; LedOff(led++));
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void PasswordLockSM_Idle(void)
{
	static PasswordStateType pCurrentPasswordState = PASSWORD_NOT_READY;
	static u32 u32TimeStamp = 0;
	static ButtonInputBufferType pButtonPassword;
	static ButtonInputBufferType pButtonInputBuffer;
	static bool bInputOverfill = FALSE;
	static bool bStateInit = FALSE;
	
	if(u32TimeStamp == 0)
	{
		u32TimeStamp = G_u32SystemTime1ms;
		pButtonPassword.u8Size = U8_DEFAULT_PASSWORD_LENGTH;
		pButtonPassword.p_arryBuffer =  malloc(sizeof(ButtonInputBufferType)*U8_MAX_PASSWORD_SIZE);
		pButtonInputBuffer.p_arryBuffer =  malloc(sizeof(ButtonInputBufferType)*(U8_MAX_PASSWORD_SIZE+1));
		for(int i = 0; i < U8_DEFAULT_PASSWORD_LENGTH; i++)
		{
			pButtonPassword.p_arryBuffer[i] = P_ARY_DEFAULT_PASSWORD[i];
		}
		
	}
	
	if(pCurrentPasswordState == PASSWORD_NOT_READY && G_u32SystemTime1ms - u32TimeStamp >= 2000)
	{
		bStateInit = TRUE;
		PWMAudioSetFrequency(BUZZER1, 300);
		if(waitingForBtn(BUTTON3))
		{
			//Initaialize for Change State
			pCurrentPasswordState = PASSWORD_CHANGE;
			u32TimeStamp = G_u32SystemTime1ms;
			LedOn(ORANGE);
		}
		else
		{
			pCurrentPasswordState = PASSWORD_LOCKED;
		}
	}
	
	//Manages password buttons when PASSWORD_READY
	if(pCurrentPasswordState != PASSWORD_NOT_READY)
	{
		
		if(bInputOverfill == TRUE && G_u32SystemTime1ms - u32TimeStamp >= 700)
		{
			PWMAudioOff(BUZZER1);
			bInputOverfill = FALSE;
		}
		
		updateBtnInputBuffer(&pButtonInputBuffer);
		if(pButtonInputBuffer.u8Size > U8_MAX_PASSWORD_SIZE)
		{
			pButtonInputBuffer.u8Size = 0;
			bInputOverfill = TRUE;
			PWMAudioOn(BUZZER1);
			u32TimeStamp = G_u32SystemTime1ms;
		}
	}
	
	if(pCurrentPasswordState == PASSWORD_CHANGE)
	{
		//Begin alternating green and red leds
		if(bStateInit == TRUE && G_u32SystemTime1ms - u32TimeStamp == 1500)
		{
			LedOff(ORANGE);
			LedBlink(GREEN, LED_2HZ);
		}
		if(bStateInit && G_u32SystemTime1ms - u32TimeStamp == 1750)
		{
			LedBlink(RED, LED_2HZ);
			bStateInit = FALSE;
		}
		
		if(waitingForBtn(BUTTON3))
		{
			if(pButtonInputBuffer.u8Size == 0)
			{
				bInputOverfill = TRUE;
				PWMAudioOn(BUZZER1);
				u32TimeStamp = G_u32SystemTime1ms;
			}
			else
			{
			for(int i = 0; i < pButtonInputBuffer.u8Size; i++)
			{
				pButtonPassword.p_arryBuffer[i] = pButtonInputBuffer.p_arryBuffer[i];
			}
			pButtonPassword.u8Size = pButtonInputBuffer.u8Size;
			pButtonInputBuffer.u8Size = 0;
			bStateInit = TRUE;
			pCurrentPasswordState = PASSWORD_LOCKED;
			clearLeds();
			}
			
		}
	}
	
	if(pCurrentPasswordState == PASSWORD_LOCKED)
	{
		if(bStateInit == TRUE)
		{
			LedOn(RED);
			bStateInit = FALSE;
		}
		if(checkPassword(&pButtonInputBuffer, &pButtonPassword) == 1){
			bStateInit = TRUE;
			pCurrentPasswordState = PASSWORD_UNLOCKED;
			clearLeds();
		}
	}
	
	if(pCurrentPasswordState == PASSWORD_UNLOCKED)
	{
		if(bStateInit == TRUE)
		{
			bStateInit = FALSE;
			LedOn(GREEN);
		}
		if(waitingForBtn(BUTTON3))
		{
			pButtonInputBuffer.u8Size = 0;
			clearLeds();
			bStateInit = TRUE;
			pCurrentPasswordState = PASSWORD_LOCKED;
		}
	}
} /* end PasswordLockSM_Idle() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void PasswordLockSM_Error(void)          
{
	static bool bInitializeError = TRUE;
	if(bInitializeError == TRUE)
	{
		OutputManagerSwitchOutputState(LED_OUTPUT_RGFLASH);
		bInitializeError = FALSE;
	}
} /* end PasswordLockSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
