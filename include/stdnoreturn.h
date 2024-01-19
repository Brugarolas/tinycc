#ifndef _STDNORETURN_H
#define _STDNORETURN_H


/* ISOC11 noreturn */
#define noreturn _Noreturn

#if __STDC_VERSION__ >= 202310
    #warning The stdnoreturn.h is deprecated
#endif

#endif /* _STDNORETURN_H */
