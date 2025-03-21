#ifndef OPERATION_H
#define OPERATION_H

/** The instructions themselves -- feel free to subclass instead */
typedef enum {
    ldconst, //var=const
    copy, //var0=var0
    add, //var0=var0+var1
    sub, //var0=var0-var1
    mul, //var0=var0*var1
    div_, //var0=var0/var1
    mod, //var0=var0%var1
    notU, //var0=!var0
    negU, //var0=-var0
    andbb, //var0=var0&var1
    notbb, //var0=~var0
    orbb, //var0=var0|var1
    rmem, //dest=*addr
    wmem, //*var0=var0
    call, 
    cmp_eq, //var0=(var0==var1)
    cmp_lt, //var0=(var0<var1)
    cmp_gt, //var0=(var0>var1)
    cmp_le, //var0=(var0>var1)
    cmp_neq, //var0=(var0!=var1)
} Operation;

#endif
