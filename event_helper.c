
#include <xc.h>
#include <string.h>
#include "i2c.h"
#include "AT24CS04.h"
#include "clcd.h"
#include "timers.h"
#include "main.h"
#include "matrix_keypad.h"
#include "uart.h"

//global variables and arrays declared
int pos = -1;
int access = -1;
int lap = -1;

char rpos = 0;
char start = 0, lap_flag = 0;

char log[17];
char string[]  = "SW:  00:00:00:00";
char default_string[] = "SW:  00:00:00:00";
char lap_string[] = "L0:";

char * menu[] = {"View Logs", "Download Logs", "Clear Logs"};

extern unsigned char control_flag;
extern unsigned char reset_flag;
unsigned char count = 0;

void log_event(void)
{
    unsigned char add;
    if(++pos == 5)
        pos = 0;
    
    //0, 16, 32 ... 
    //16 bytes of data always written as "SW:  00:00:00:00"
    //logging/tracking total 10 events soo position increments by 16 each time uptill 80
    add = (pos * 16);
    
    //0-15, 16-31, ... 64-80
    //write into ext EEPROM
    write_str_AT24CS04(add, log);
    
    //to keep track of events 
    //if access occurs it means an event has occured
    if(access < 4)
        access++;
}
void log_main_event(char * event)
{
    //SW:  HH:MM:SS:CS -> 16 bytes
    strcpy(log, event);
      
    //call log_event function to write to EEPROM
    log_event();
    
}
void display(unsigned char key)
{
    //static unsigned char pre_key;
    
    static char string_copy[20];
    
    if (key != ALL_RELEASED)
    {
        //switch between 
        switch (key)
        {
            // start/stop key
            case 1:
                start = 1;
                lap_flag = 1;
                //pre_key = key;
                TMR2ON = 1;
                clcd_print("                ", LINE2(0));
                count++;
                //if count == 2 it means that switch1 was used
                //to stop the watch, soo now cant log events unless 
                //watch started again
                if(count == 2)
                {
                    count = 0;
                    start = 0;
                    lap_flag = 0;
                }
                break;
             
            //lap key
            case 2:
                //stop = 1;
                //start = 0;
                //lap_flag = 1;
                strcpy(string_copy, &string[5]);
                
                log_main_event(string);
                
                strcpy(string, default_string);
                //if(start != 0)
                if(lap_flag)
                    lapDisplay(lap_string, string_copy);
                break;
            case 3:
                control_flag = LOGIN_MENU_FLAG;
                reset_flag = RESET_LOGIN_MENU;
                break;
            default:
                //clcd_putch('*', LINE2(count_line));
                break;
        }
 
    }
    if(start)
    {
        //start_display(pre_key);
        clcd_print(string, LINE1(0));
    }
    else if(!start)
    {
        TMR2ON = 0;
    }
   
}
/*
void start_display(unsigned char key)
{
    
     
    if(centi_seconds == 100)
    {
        seconds++;
    
        if(seconds == 60)
        {
            if(++minuites == 60)
            {
                minuites = 0;
                if(++hours == 24)
                {
                    hours = 0;
                }
                ssd[0] = hours / 10;
                ssd[1] = hours % 10;
            }
            ssd[2] = minuites / 10;
            ssd[3] = minuites % 10;
            seconds = 0;
        }
        centi_seconds = 0;
    }
    
}
*/
void lapDisplay(char * lap_string, char * string_copy)
{
    
    if(++lap < 5)
    {
        lap_string[1] = (unsigned char)lap + '0';
        clcd_print(lap_string, LINE2(0));
        clcd_print(string_copy, LINE2(5));
      
    }
    else
    {
        lap = -1;
    }
}

//function to select option in menu_bar and return desired position of '*'
char login_menu(unsigned char key, unsigned char reset_flag)
{
    static char menu_pos;
    static char select_pos;
    //reset variables upon re-entry
    if(reset_flag == RESET_LOGIN_MENU)
    {
        select_pos = 1;
        menu_pos = 0;
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
        __delay_us(500);
    }
    //increment/decrement menu position on switch press
    if((key == SW2) && menu_pos < 2)
    {
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
        __delay_us(500);
        menu_pos++;
        select_pos = 2;
    }
    else if((key == SW1) && menu_pos > 0)
    {
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
        __delay_us(500);
        menu_pos--;
        select_pos = 1;
    }
    // char '*' helps user to identify selected position on clcd display
    if(select_pos == 1)
    {
        clcd_putch('*', LINE1(0));
        clcd_print(menu[menu_pos], LINE1(2));
        clcd_print(menu[menu_pos + 1], LINE2(2));
    }
    else if(select_pos == 2)
    {
        clcd_putch('*', LINE2(0));
        clcd_print(menu[menu_pos - 1], LINE1(2));
        clcd_print(menu[menu_pos], LINE2(2));
    }
    return menu_pos;

}

void view_log(unsigned char key, unsigned char reset_flag)
{
    static char rlog[16];
    static unsigned char flag = 1, flag_clear;
    static unsigned char add;
    if(access == -1)
    {
        clcd_print("No Logs", LINE1(0));
        flag_clear = 1;
        return;
    }
    else
    {
        if(reset_flag == RESET_VIEW_LOG_POS)
        {
            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            __delay_us(100);
            add = 5;
            rpos = 0;
            key = 0x00;
        }
        if(flag_clear)
        {
            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            __delay_us(500);
            flag_clear = 0;
        }
        if(flag)
        {
            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            __delay_us(500);
            flag = 0;
        }
        
        if((SW1 == key) && (rpos < access))
        {
            rpos++;
        }
        else if((SW2 == key) && (rpos > 0))
        {
            rpos--;
        }
        
        add = rpos * 16;
        
        for(unsigned char i = 0; i < 16; i++)
        {
            rlog[i] = read_AT24CS04(add + i);
        }
        //print the logs on the clcd screen
        //logs contains SW:  HH:MM:SS:CS
        clcd_putch(rpos + '0', LINE2(0));
        clcd_putch(rlog[5], LINE2(2));
        clcd_putch(rlog[6], LINE2(3));
        clcd_putch(rlog[7], LINE2(4));
        clcd_putch(rlog[8], LINE2(5));
        clcd_putch(rlog[9], LINE2(6));
        clcd_putch(rlog[10], LINE2(7));
        clcd_putch(rlog[11], LINE2(8));
        clcd_putch(rlog[12], LINE2(9));
        clcd_putch(rlog[13], LINE2(10));
        clcd_putch(rlog[14], LINE2(11));
        clcd_putch(rlog[15], LINE2(12));
        
        
    }
}

unsigned char download_logs(void)
{
    static unsigned char dlogs[16];
    
    static unsigned char add, position = 0;
    
    
    if(access == -1)
    {
        puts("No logs");
        position = 0;
        clcd_print("   Stop Watch", LINE1(0)); 
        clcd_print(" Key 1 to start", LINE2(0));
        start = 1;
        //stop = 0;
        lap_flag = 0;
        TMR2ON = 0;
        count = 0;
        strcpy(string, default_string);
        return DASH_BOARD_FLAG;
    }
    
    else
    {
        add = position * 16;
        
        for(unsigned char i = 0; i < 16; i++)
            dlogs[i] = read_AT24CS04(add + i);
        
        //printing the characters from array into the serial terminal
        puts("STAT");
        putchar(position + '0');
        putchar('\n');
        putchar(dlogs[0]);
        putchar(dlogs[1]);
        putchar(dlogs[2]);
        putchar(dlogs[3]);
        putchar(dlogs[4]);
        putchar(dlogs[5]);
        putchar(dlogs[6]);
        putchar(dlogs[7]);
        putchar(dlogs[8]);
        putchar(dlogs[9]);
        putchar(dlogs[10]);
        putchar(dlogs[11]);
        putchar(dlogs[12]);
        putchar(dlogs[13]);
        putchar(dlogs[14]);
        putchar(dlogs[15]);
        putchar('\n');
        
        //once position > access then reset parameters and display default screen
        if(++position > access)
        {
            position = 0;
            clcd_print("   Stop Watch", LINE1(0)); 
            clcd_print(" Key 1 to start", LINE2(0));
            start = 0;
            //stop = 0;
            lap_flag = 0;
            TMR2ON = 0;
            count = 0;
            strcpy(string, default_string);
            return DASH_BOARD_FLAG;
            
        }
    }
    return DOWNLOAD_LOG_FLAG;
    
}

//function to clear the logs
void clear_log(void)
{
    //clearing all the global variables
    clcd_print("Cleared Logs", LINE1(0));
    access = -1;
    pos = -1;
    rpos = 0;
}