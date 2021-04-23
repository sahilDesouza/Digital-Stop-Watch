
#include <xc.h>
#include "i2c.h"

void init_i2c(unsigned long baud)
{
    /* Set PIC as I2C Master Mode */
    SSPM3 = 1;

    /*Set the Required Baudrate for our master for the  i2c communication */
    SSPADD = (FOSC / (4 * baud)) - 1;

    /*Enable SSP */
    SSPEN = 1;
}

static void i2c_wait_for_idle(void)
{
    /* Wait till no activity on the bus */
    /*ORing R/W bit of SSPSTAT with SEN, RSEN, PEN, RCEN or ACKEN bits of SSPCON2 register will indicate if the MSSP is
in Idle mode.*/
    while (R_nW || (SSPCON2 & 0x1F));
}

/*note: SEN,RSEN,PEN bits are set by us, but automatically cleared by hardware*/

void i2c_start(void)
{
    /*Check if bus is free before starting commmunication*/
    i2c_wait_for_idle();

    /*1 = Initiate Start condition on SDA and SCL pins. Automatically cleared by hardware.*/
    SEN = 1;
}

void i2c_rep_start(void)
{
    i2c_wait_for_idle();
    RSEN = 1;
}

void i2c_stop(void)
{
    i2c_wait_for_idle();
    PEN = 1;
}

unsigned char i2c_read(unsigned char ack)
{
    unsigned char data;

    i2c_wait_for_idle();
    /*1 = Enables Receive mode for I 2 C*/
    RCEN = 1;

    i2c_wait_for_idle();
    data = SSPBUF;

    if (ack == 1)
    {
        /*Ack the slave*/
        ACKDT = 1;
    }
    else
    {
        ACKDT = 0;
    }

    ACKEN = 1;

    return data;
}

int i2c_write(unsigned char data)
{
    i2c_wait_for_idle();
    SSPBUF = data;

    /*ACKSTAT: 0 = Acknowledge was received from slave*/
    return !ACKSTAT;
}