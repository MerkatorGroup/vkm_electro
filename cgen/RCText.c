/**-----------------------------------------------
 * RCText.c
 * This file is automatically generated, don't try to edit it
 * all modifications will be lost with next project saving
 **-----------------------------------------------*/

#include <stdio.h>
#include <RCText.h>
#include <UserCAPI.h>

#define MIN_TEXT_OFFSET 289888
#define MAX_TEXT_OFFSET 289888

static char buffer[64];

//-------------------------------------------------
const char *RCTextGetText(uint32_t offset, uint32_t lngIdx)
{
  uint32_t numLang;
  uint32_t offsetToString;
  
  const uint8_t *ResourceBaseAdr = GetResDataBase();
  const uint32_t *TextResItem = (const uint32_t *)(const void *)(ResourceBaseAdr + offset);
  
  numLang = TextResItem[0];
  
  if(lngIdx >= numLang)
  {
    lngIdx = numLang - 1;
  }
  
  offsetToString = TextResItem[lngIdx + 1];
  
  if((offset<MIN_TEXT_OFFSET) || (offset>MAX_TEXT_OFFSET))
  {
    sprintf(buffer,"Invalid pointer 0x%08x to text",offset);
    return buffer;
  }
  else
  {
    return ((const char*)TextResItem) + offsetToString;
  }
}

