//******************************************************************
//* Driver Callback API                                             
//* Do not modify !                                                 
//* This file will be overwritten at next project export !          
//******************************************************************

#include <UserCAPI.h>
typedef void(*tPt2Func)(void);

void _gs_event_keytelegram_event(void);
void _gs_event_keytelegram_preinit(void);

tPt2Func EXPORTFROMSO __gs__funcs_event[] =
{
  _gs_event_keytelegram_event,
  0
};

tPt2Func EXPORTFROMSO __gs__funcs_preinit[] =
{
  _gs_event_keytelegram_preinit,
  0
};

