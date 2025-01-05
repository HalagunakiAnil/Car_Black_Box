#include <xc.h>


#define         LEVEL           0
#define         STATE           1

#define         R1              RB5
#define         R2              RB6
#define         R3              RB7

#define         C1              RB1
#define         C2              RB2
#define         C3              RB3
#define         C4              RB4

#define         ALL_RELEASED    0x00

#define         SW1             1
#define         SW2             2
#define         SW3             3
#define         SW4             4
#define         SW5             5

void init_mkp(void);
unsigned char read_matrix_keypad(unsigned char detection_type);
unsigned char scan_key(void);

