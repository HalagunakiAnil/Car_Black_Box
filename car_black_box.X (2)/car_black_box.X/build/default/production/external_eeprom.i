# 1 "external_eeprom.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:/Program Files/Microchip/MPLABX/v6.00/packs/Microchip/PIC18Fxxxx_DFP/1.3.36/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "external_eeprom.c" 2
# 1 "./external_eeprom.h" 1







void write_external_eeprom(unsigned char address1, unsigned char data);
unsigned char read_external_eeprom(unsigned char address1);
# 1 "external_eeprom.c" 2

# 1 "./main.h" 1
# 10 "./main.h"
enum menu_operation
{
    DASH_BOARD,
    MAIN_MENU,
    VIEW_LOG,
    CLEAR_LOG,
    DOWNLOAD_LOG,
    SET_TIME,

};

void dash_board(void);
void clear_log(void);
void download_log(void);
void set_time(void);
void store_event(void);
void menu_display(void);
# 2 "external_eeprom.c" 2

# 1 "./i2c.h" 1




void init_i2c(void);
void i2c_start(void);
void i2c_rep_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(void);
# 3 "external_eeprom.c" 2


void write_external_eeprom(unsigned char address, unsigned char data)
{
 i2c_start();
 i2c_write(0xA0);
 i2c_write(address);
 i2c_write(data);
 i2c_stop();
    for(unsigned long wait = 1000; wait--;);
}

unsigned char read_external_eeprom(unsigned char address)
{
 unsigned char data;

 i2c_start();
 i2c_write(0xA0);
 i2c_write(address);
 i2c_rep_start();
 i2c_write(0xA1);
 data = i2c_read();
 i2c_stop();

 return data;
}
