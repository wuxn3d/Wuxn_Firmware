#include "FactoryReset.h"
#include "includes.h"


void wFactoryResetUserSaidYes(void);

void wFactoryReset(void)
{
  setDialogText(LABEL_FACTORY_RESET, LABEL_FR_BOX_V1, LABEL_YES, LABEL_NO);
  showDialog(DIALOG_TYPE_INFO, wFactoryResetUserSaidYes, NULL, NULL);
  loopProcess();
}

void wFactoryResetUserSaidYes(void)
{
  SETTINGS now = infoSettings;
  infoSettings.factory_reset = ENABLE;
  
  infoSettings.enable_oobwcw = ENABLE;

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
  
  mustStoreCmd("M502\n");
  mustStoreCmd("M500\n");
  setDialogText(LABEL_FACTORY_RESET, LABEL_FR_BOX_V2, LABEL_BACKGROUND, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
}




