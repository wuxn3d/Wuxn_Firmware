#ifndef _VFS_H_
#define _VFS_H_

#include "stdint.h"
#include "stdbool.h"
#include "ff.h"

/*
#ifdef WUXN_CODE
  extern unsigned long wAfterBootFileCheckTimeOut;
  extern bool f_w_ReadyToCheckDrives;
  extern bool f_w_CheckAfterBootFileTime;
  
#endif
*/

#define FOLDER_NUM   255
#define FILE_NUM     255
#define MAX_PATH_LEN 1024

typedef enum
{
  TFT_SD,
  TFT_UDISK,
  BOARD_SD,
  BOARD_SD_REMOTE
} FS_SOURCE;

typedef struct
{
  TCHAR title[MAX_PATH_LEN];   // selected file path
  TCHAR * folder[FOLDER_NUM];  // folder list buffer
  TCHAR * file[FILE_NUM];      // file list buffer
  uint16_t folderCount;        // current folder count
  uint16_t fileCount;          // current gcode file count
  uint16_t cur_page;           // current display page index (5 files per page)
  uint16_t fileIndex;          // selected file index
  FS_SOURCE source;            // The source of the file. TFT SD or ONBOARD SD.
  TCHAR * Longfile[FILE_NUM];  // Long file name buffer from ONBOARD SD only
} MYFILE;

extern MYFILE infoFile;

bool mountFS(void);
bool scanPrintFiles(void);

TCHAR* getCurFileSource(void);
void clearInfoFile(void);
void resetInfoFile(void);
bool EnterDir(char *nextdir);
void ExitDir(void);
bool IsRootDir(void);

bool isVolumeExist(uint8_t src);
void loopVolumeSource(void);

/* NO LONGER USED  010522 WUXN PETE
#ifdef WUXN_CODE
void wSetBootComplete();
#endif
*/

#endif
