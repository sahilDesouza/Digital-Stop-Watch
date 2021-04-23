

#include <xc.h>
#include "matrix_keypad.h"

void init_matrix_keypad(void)
{
    /* Setting the Columns as Inputs (RB2 - RB0)*/
    MATRIX_KEYPAD_COLUMN_PORT_DDR = MATRIX_KEYPAD_COLUMN_PORT_DDR | 0x07;
    /* Setting the Rows as Output (RD3 - RD0)*/
    MATRIX_KEYPAD_ROW_PORT_DDR = MATRIX_KEYPAD_ROW_PORT_DDR & 0xF0;
    
    /* Enabling PORTB Pullups */
    nRBPU = 0;
    
    /* Making all the Rows HIGH to start with */
    ROW1 = HI;
    ROW2 = HI;
    ROW3 = HI;
    ROW4 = HI;
}

static unsigned char scan_keypad(void)
{
    int i;
    
    //1st case
    ROW1 = LOW;
    ROW2 = HI;
    ROW3 = HI;
    ROW4 = HI;   
    
    for (i = 100; i--; );
    
    //check possibility for input to anyone of the columns
    //if LOW detected then corresponding switch pressed
    //hence return the desired value
    if (COL1 == LOW)
    {
        return 1;
    }
    else if (COL2 == LOW)
    {
        return 2;
    }
    else if (COL3 == LOW)
    {
        return 3;
    }
    
    //2nd case
    ROW1 = HI;
    ROW2 = LOW;
    ROW3 = HI;
    ROW4 = HI;   
    
    for (i = 100; i--; );
    
    if (COL1 == LOW)
    {
        return 4;
    }
    else if (COL2 == LOW)
    {
        return 5;
    }
    else if (COL3 == LOW)
    {
        return 6;
    }
    
    //3rd case
    ROW1 = HI;
    ROW2 = HI;
    ROW3 = LOW;
    ROW4 = HI;   

    for (i = 100; i--; );
    
    if (COL1 == LOW)
    {
        return 7;
    }
    else if (COL2 == LOW)
    {
        return 8;
    }
    else if (COL3 == LOW)
    {
        return 9;
    }
    
    //4th case
    ROW1 = HI;
    ROW2 = HI;
    ROW3 = HI;
    ROW4 = LOW;   

    for (i = 100; i--; );
    
    if (COL1 == LOW)
    {
        return '*';
    }
    else if (COL2 == LOW)
    {
        return 0;
    }
    else if (COL3 == LOW)
    {
        return '#';
    }
    
    return ALL_RELEASED;
}

unsigned char read_matrix_keypad(unsigned char mode)
{
    static unsigned char once = 1;
    static unsigned int long_keypress;
    static unsigned char pre_key,  key;
    
    key = scan_keypad();
    
    if (mode == LEVEL)
    {
        return key;
    }
    else
    {
        if ((key != ALL_RELEASED) && once)
        {
            once = 0;
            pre_key = key;
            long_keypress = 0;
            //return key;
        }
        else if((pre_key == key) && (long_keypress < 40) && !once)
        {
            long_keypress++;
        }
        else if(long_keypress == 40)
        {
            long_keypress++;
            return pre_key | 0x80;
        }
        else if (key == ALL_RELEASED && !once)
        {
            once = 1;
            if(long_keypress < 40)
            {
                return pre_key;
            }
        }
    }
    
    return ALL_RELEASED;
}
