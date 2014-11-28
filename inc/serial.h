#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#define COM1 0x3F8
#define SER_DIV 3

void init_serial(void);
int serial_received(void); 
char read_serial(void);
int is_transmit_empty(void);
void write_serial(char);

#endif