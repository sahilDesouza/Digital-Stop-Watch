/* 
 * File:   AT24CS04.h
 * Author: duane
 *
 * Created on 14 July, 2020, 5:21 PM
 */

#ifndef AT24CS04_H
#define	AT24CS04_H

#define SLAVE_WRITE             0b10100000 //A2 and A1 pins grouded//
#define SLAVE_READ              0b10100001


void init_AT24CS04(void);
unsigned char read_AT24CS04(unsigned char addr);
void write_AT24CS04(unsigned char addr, unsigned char data);
void write_str_AT24CS04(unsigned char addr, char * data);
#endif	/* AT24CS04_H */

