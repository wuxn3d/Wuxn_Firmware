#include "serialConnection.h"
#include "includes.h"

bool serialHasBeenInitialized = false;
const u32 baudrateList[BAUDRATE_COUNT] = {2400, 9600, 19200, 38400, 57600, 115200, 250000, 500000, 1000000};

void Serial_ReSourceDeInit(void)
{
  if (!serialHasBeenInitialized) return;
  serialHasBeenInitialized = false;
  Serial_DeInit();
}

void Serial_ReSourceInit(void)
{
  if (serialHasBeenInitialized) return;
  serialHasBeenInitialized = true;

  memset(&infoHost, 0, sizeof(infoHost));
  reminderSetUnConnected();  
  Serial_Init(baudrateList[infoSettings.baudrate]);
}
