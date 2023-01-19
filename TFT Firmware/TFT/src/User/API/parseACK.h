#ifndef _PARSEACK_H_
#define _PARSEACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "Configuration.h"
#include "includes.h"

static const char errormagic[] = "Error:";

static const char echomagic[] = "Message:";
static const char warningmagic[] = "Warning:";                     
static const char messagemagic[] = "message";                      
static const char errorZProbe[] = "ZProbe triggered before move";  

#define ACK_MAX_SIZE 512

typedef enum  
{
  ECHO_NOTIFY_NONE = 0,  
  ECHO_NOTIFY_TOAST,     
  ECHO_NOTIFY_DIALOG,    
} ECHO_NOTIFY_TYPE;

typedef enum  
{             
  ECHO_ID_BUSY = 0,
  ECHO_ID_FRESH_FILE,
  ECHO_ID_DOING_FILE,
  ECHO_ID_PROBE_OFFSET,
  ECHO_ID_FLOW,
  ECHO_ID_ECHO,
  ECHO_ID_ECHO_G,
  ECHO_ID_ECHO_M,
  ECHO_ID_CAP,
  ECHO_ID_CONFIG,
  ECHO_ID_SETTINGS,
  ECHO_ID_SOFT_ENDSTOP,
  ECHO_ID_BED_LEVELING,
  ECHO_ID_FADE_HEIGHT,
  ECHO_ID_TOOL_CHANGE,
  ECHO_ID_COUNT,
} ECHO_ID;

typedef struct
{
  ECHO_NOTIFY_TYPE  notifyType;
  const char *const msg;
} ECHO;

bool dmaL1NotEmpty(uint8_t port);
void setCurrentAckSrc(uint8_t src);
void parseACK(void);
void parseRcvGcode(void);

void setIgnoreEcho(ECHO_ID msgId, bool state);

#ifdef __cplusplus
}
#endif

#endif
