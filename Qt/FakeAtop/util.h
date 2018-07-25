#ifndef UTIL_H
#define UTIL_H

#include <linux/types.h>



struct ifprop	{
    char		type;		/* type: 'e' (ethernet)		*/
                    /*       'w' (wireless)  	*/
    char		name[31];	/* name of interface  		*/
    long int	speed;		/* in megabits per second	*/
    char		fullduplex;	/* boolean			*/
};
#endif // UTIL_H
