#ifndef QUICKSORTMP_SWAP_H
#define QUICKSORTMP_SWAP_H

#include <stdbool.h>

#define SWAP(__swap_lhs, __swap_rhs, __swap_size)                                                 \
    do {                                                                                          \
        register size_t __size = (__swap_size);                                                   \
        register unsigned char *__lhs = (__swap_lhs);                                             \
        register unsigned char *__rhs = (__swap_rhs);                                             \
        do                                                                                        \
          {                                                                                       \
            register unsigned char __tmp = *__lhs;                                                \
            *__lhs++ = *__rhs;                                                                    \
            *__rhs++ = __tmp;                                                                     \
          } while (--__size > 0);                                                                 \
      } while (false)



#endif //QUICKSORTMP_SWAP_H
