#ifndef _STDBOOL_H
#define _STDBOOL_H

/* ISOC99 boolean */

#define bool	_Bool
#define true	1
#define false	0
#define __bool_true_false_are_defined 1


#if __STDC_VERSION__ >= 202311
#warning <stdbool.h> is deprecrated. We strongly recommend that you do not use this header file in conjunction with ISOC23
#endif


#endif /* _STDBOOL_H */
