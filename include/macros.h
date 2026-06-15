#ifndef MACROS_H
#define MACROS_H

/* Basic bit operations */
#define SET_BIT(REG, BIT)     ((REG) |= (1U << (BIT)))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(1U << (BIT)))
#define TOGGLE_BIT(REG, BIT)  ((REG) ^= (1U << (BIT)))
#define READ_BIT(REG, BIT)    (((REG) >> (BIT)) & 0x1U)
/* Check if a specific bit is set in any register and return true if yes */
#define BIT_IS_SET(REG,BIT) ( REG & (1<<BIT) )

/* Check if a specific bit is cleared in any register and return true if yes */
#define BIT_IS_CLEAR(REG,BIT) ( !(REG & (1<<BIT)) )


#endif /* MACROS_H */
