#ifndef _STDBOOL_H
#define _STDBOOL_H

/* ISOC99 boolean */

#if __STDC_VERSION__ != 202310 

#define bool	_Bool
#define true	1
#define false	0
#define __bool_true_false_are_defined 1

#endif

#endif /* _STDBOOL_H */
