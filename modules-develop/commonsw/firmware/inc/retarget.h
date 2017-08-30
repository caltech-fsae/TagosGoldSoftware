/* 
 * retargetted dependencies
 */

#ifndef __RETARGET_H
#define __RETARGET_H

#include <stdio.h>

/* fputc UART redirections */
extern FILE __stdlog, __stdbeam, __stddebug;
#define stdlog &__CLIBNS __stdlog
#define stdbeam &__CLIBNS __stdbeam
#define stddebug &__CLIBNS __stddebug

#endif /* __RETARGET_H */
