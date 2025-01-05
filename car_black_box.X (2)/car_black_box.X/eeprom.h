#ifndef EEPROM_H
#define EEPROM_H

/*Function definitions*/
void write_internal_eeprom(unsigned char address, unsigned char data); 
unsigned char read_internal_eeprom(unsigned char address);

#endif