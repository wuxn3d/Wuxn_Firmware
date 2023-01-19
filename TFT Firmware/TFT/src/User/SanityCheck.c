#include "SanityCheck.h"


void sizecheck(void)
{
    
  SIZE_CHECK((sizeof(SETTINGS) + 16) > PARA_SIZE);                   
  SIZE_CHECK(sizeof(STRINGS_STORE) > STRINGS_STORE_MAX_SIZE);        
  SIZE_CHECK(sizeof(CUSTOM_GCODES) > CUSTOM_GCODE_MAX_SIZE);         
  SIZE_CHECK(sizeof(PRINT_GCODES) > PRINT_GCODES_MAX_SIZE);          
  SIZE_CHECK((LABEL_NUM * MAX_LANG_LABEL_LENGTH) > LANGUAGE_SIZE);   
}
