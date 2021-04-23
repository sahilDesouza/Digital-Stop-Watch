/*
 * File:   main.c
 * Author: Sahil Desouza_20001
 * Description:  A Digital stop watch is a hand held timepiece designed to measure the amount of time elapses between
 *               its activation and deactivation. The Project implemented mimics the same functionality as a digital stop watch.
 *               "one" push button on the matrix keypad is responsible to start and stop the time which is displayed on the startup dashboard.
 *               Number "two" on the matrix keypad was used for lapping the time. Each time a lap event occurred data was stored on an
 *               External EEPROM(AT24CS04) using i2c protocol, 5 laps in total were logged. The menu login UI helps to display all the  
 *               logged events on the CLCD itself or it can be displayed on a terminal using USART protocol, an option to clear the logs    
 *               in the EEPROM is also present. Navigation through this menu UI is done using the matrix keypad itself and selection 
 *               of a particular menu option is possible with a long keypress. Navigating back and forth can also be done in a similar way. 
 *
 * Created on 20 April, 2021, 9:47 AM
 */


#include <xc.h>
#include <string.h>
#include "clcd.h"
#include "matrix_keypad.h"
#include "timers.h"
#include "main.h"
#include "i2c.h"
#include "uart.h"

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)

//Variable declration
//int flag, count, lap = -1;
//unsigned char arr[5][17];

//   srt[]  = "SW:  HH:MM:SS:CS"

//global/extern declared
unsigned char control_flag, reset_flag;

//char str1[] = "L0:  ";
extern char start, lap_flag;
extern char string[];
extern char default_string[];
extern unsigned char count;

//function to initialise all the peripherals
static void init_config(void) 
{
    init_clcd();
    init_matrix_keypad();
    init_timer2();
    init_i2c(9600);
    init_uart(9600);
    GIE = 1; /*Enable all the global interrupts*/
    PEIE = 1;
    
    //Print here initial output display as per the requirement
    clcd_print("   Stop Watch", LINE1(0)); //LINE1
    clcd_print(" Key 1 to start", LINE2(0)); //LINE2
}

void main(void) 
{
    
    init_config();
    
    //local variables
    unsigned char key;
    unsigned char menu_pos;
    
    //control flag used to help display that particular option
    //and to navigate through the UI.
    control_flag = DASH_BOARD_FLAG;

    while (1) 
    {
        //gets input from a matrix keypad
        key = read_matrix_keypad(STATE);
        
        //hold switch2 to exit from respective option present in menu_bar that user had recently entered
        if((key == LSW2) && ((control_flag == VIEW_LOG_FLAG) || (control_flag == CLEAR_LOG_FLAG) || (control_flag == DOWNLOAD_LOG_FLAG)))
        {
            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            __delay_us(100);
            control_flag = LOGIN_MENU_FLAG;
        }
        //hold switch2 to exit from login menu back and go back to the main dashboard 
        else if((key == LSW2) && control_flag == LOGIN_MENU_FLAG)
        {
            //display default UI and reset respective parameters
            control_flag = DASH_BOARD_FLAG;
            clcd_print("   Stop Watch", LINE1(0)); 
            clcd_print(" Key 1 to start", LINE2(0));
            start = 0;
            
            lap_flag = 0;
            TMR2ON = 0;
            count = 0;
            strcpy(string, default_string);
        }
        
        //hold switch1 to enter into respective option present in menu_bar
        if((LSW1 == key) && (control_flag == LOGIN_MENU_FLAG))
        {
            //when in the menu bar based on the menu_position respective control flag value is updated
            switch(menu_pos)
            {
                case 0:
                    //to view log
                    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                    __delay_us(100);
                    control_flag = VIEW_LOG_FLAG;
                    //reset flag is used to reset respective varables back to default
                    //when not required RESET_Nothing macro is used
                    reset_flag = RESET_VIEW_LOG_POS;
                    break;
                
                case 1:
                    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                    __delay_us(100);
                    clcd_print("DOWLOADING...", LINE1(0));
                    __delay_ms(1000);
                    //clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                    //__delay_us(100);
                    puts("LOGS: SW:  HH:MM:SS:CS\n\r");
                    //change control flag to DOWNLOAD_LOG_FLAG
                    control_flag = DOWNLOAD_LOG_FLAG;
                    break;
                
                case 2:
                    //change control flag to clearlog
                    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                    __delay_us(100);
                    control_flag = CLEAR_LOG_FLAG;
        
            }
        }
        
        //this control flag is used to navigate and run the respective function
        //based on the choice made by the user
        switch(control_flag)
        {
            //this is basically the default switch case
            case DASH_BOARD_FLAG:
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                
                //initially main display function is run
                display(key);
                if(reset_flag == RESET_LOGIN_MENU)
                {
                    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                    __delay_us(100);
                    continue;
                }
                break;
                
            case LOGIN_MENU_FLAG:
                //function that returns menu_pos when navigating through the options present
                menu_pos = login_menu(key, reset_flag);
                break;
                
            case VIEW_LOG_FLAG:
                //function call to display all data stored in the external EEPROM
                view_log(key, reset_flag);
                break;
                
            case DOWNLOAD_LOG_FLAG:
                //function to display all data stored in the external EEPROM in the serial terminal
                control_flag = download_logs();
                break;
            case CLEAR_LOG_FLAG:
                //function to clear data from the EEPROM
                clear_log();
                break;
        }
        
        //reset flag is used to reset respective varables back to default
        //when not required RESET_Nothing macro is used
        reset_flag = RESET_NOTHING;
        
    }
 
}
