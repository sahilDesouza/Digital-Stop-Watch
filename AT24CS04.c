

#include "i2c.h"
#include "AT24CS04.h"
#include <xc.h>
#include <string.h>
#include "clcd.h"
#include "matrix_keypad.h"
#include "timers.h"
#include "main.h"
unsigned char read_AT24CS04(unsigned char addr)
{
    unsigned char data;

    i2c_start();

    /*Write slave id with write intention*/
    //write to desired slave with intentions(read/write))
    //7 bit address + 1 bit W
    //Master informing slave that it wants to write to it
    i2c_write(SLAVE_WRITE);

    i2c_write(addr);

    i2c_rep_start();

    /*Write slave id with read intention*/
    i2c_write(SLAVE_READ);

    //0 is ack 
    data = i2c_read(0);

    i2c_stop();

    return data;
}

void write_AT24CS04(unsigned char addr, unsigned char data)
{
    i2c_start();
    i2c_write(SLAVE_WRITE); //slave id with write intention
    i2c_write(addr);        //addr where data is to be written
    i2c_write(data);
    i2c_stop();
}
void write_str_AT24CS04(unsigned char addr, char * data)
{
    while(*data)
    {
        //i2c_start();
        //i2c_write(SLAVE_WRITE); //slave id with write intention
        //i2c_write(addr);        //addr where data is to be written
        write_AT24CS04(addr, *data);
        data++;
        addr++;
    }
    i2c_stop();
}

