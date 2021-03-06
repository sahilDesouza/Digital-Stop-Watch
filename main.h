/* 
 * File:   main.h
 * Author: sahil
 *
 * Created on 7 December, 2020, 9:45 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#define LOGIN_SUCCESS              0X01
#define RETURN_BACK                0X02
#define TASK_SUCCESS               0X11
#define TASK_FAIL                  0X22


#define DASH_BOARD_FLAG            0X01
#define LOGIN_FLAG                 0X02
#define LOGIN_MENU_FLAG            0X04
#define VIEW_LOG_FLAG              0X08
#define CLEAR_LOG_FLAG             0X10
#define DOWNLOAD_LOG_FLAG          0X20
#define SET_TIME_FLAG              0X40
#define CHANGE_PASSWORD_FLAG       0X80
#define RESET_NOTHING              0X00
#define RESET_PASSWORD             0X01
#define RESET_LOGIN_MENU           0X02
#define RESET_MEMORY               0X04
#define RESET_VIEW_LOG_POS         0X08
#define RESET_TIME                 0X10
#define CONFIRM_CHANGE             0X50

#define SW1                        0x01
#define SW2                        0x02

#define LSW1                      0x81
#define LSW2                      0x82
#define LSW5                      0xAF
void log_main_event(char * event);
void log_event(void);
void display(unsigned char key);
//void start_display(unsigned char key);
void lapDisplay(char * lap_string, char * string_copy);
char login_menu(unsigned char key, unsigned char reset_flag);
void view_log(unsigned char key, unsigned char reset_flag);
unsigned char download_logs(void);
void clear_log(void);
#endif	/* MAIN_H */

