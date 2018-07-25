#include <stdio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/wireless.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <linux/socket.h>
#include <time.h>
#include <stdlib.h>
#include <linux/shm.h>
#include <regex.h>

#include "atop.h"
#include "acctproc.h"
#include "ifprop.h"
#include "photoproc.h"
#include "photosyst.h"
#include "showgeneric.h"
#include "parseable.h"

#define	allflags  "ab:cde:fghijklmnopqrstuvwxyz1ABCDEFGHIJKL:MNOP:QRSTUVWXYZ"
#define	PROCCHUNK	100	/* process-entries for future expansion  */
#define	MAXFL		64      /* maximum number of command-line flags  */


unsigned int pagesize;
/*
** declaration of global variables
*/
struct utsname	utsname;
int		utsnodenamelen;
time_t 		pretime;	/* timing info				*/
time_t 		curtime;	/* timing info				*/
unsigned long	interval = 10;
unsigned long 	sampcnt;
#ifdef _USE_NCURSES
char		screen;
#endif
int		linelen  = 80;
char		acctreason;	/* accounting not active (return val) 	*/
char		rawname[RAWNAMESZ];
char		rawreadflag;
unsigned int	begintime, endtime;
char		flaglist[MAXFL];
char		deviatonly = 1;
char      	usecolors  = 1;  /* boolean: colors for high occupation  */
char		threadview = 0;	 /* boolean: show individual threads     */
char      	calcpss    = 0;  /* boolean: read/calculate process PSS  */

unsigned short	hertz;
unsigned int	pagesize;
int 		osrel;
int 		osvers;
int 		ossub;

int		supportflags;	/* supported features             	*/
char		**argvp;
int main(int argc, char *argv[])
{
    initifprop();
    pagesize = sysconf(_SC_PAGESIZE);



    return;


}
