


#include <xc.h>
#include "main.h"
#include "timers.h"

extern unsigned char centi_seconds;
extern char string[];
void __interrupt() isr(void)
{
    static unsigned long int count = 0;
    
   //if TMR2IF == 1 it means that interrupt has occured
    if(TMR2IF == 1)
    {
        //counting in centi seconds and incrementing
        //seconds, mins, hrs.
        if(count++ == 200)
        {
            string[15]++;
            if(string[15] > '9')
            {
                string[14]++;
                string[15] = '0';
                if(string[14] > '9')
                {
                    string[12]++;
                    string[14] = '0';
                    if(string[12] > '9')
                    {
                        string[11]++;
                        string[12] = '0';
                    }
                }
            }
            //centi_seconds++;
            count = 0;
           
        }
        TMR2IF = 0;
    }
}