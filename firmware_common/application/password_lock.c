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
  if( 1 )
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

Requires:
*/

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void PasswordLockSM_Idle(void)
{
	if(G_u32SystemTime1ms == 2000)
		{
			if(waitingForBtn(BUTTON3)
			{
				
		}
} /* end PasswordLockSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void PasswordLockSM_Error(void)          
{
  
} /* end PasswordLockSM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
