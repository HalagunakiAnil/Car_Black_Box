/* 
 * File:   main.h
 * Author: Dell
 *
 * Created on 18 December, 2024, 5:49 PM
 */

#ifndef MAIN_H
#define	MAIN_H
enum menu_operation
{
    DASH_BOARD,
    MAIN_MENU,
    VIEW_LOG,
    CLEAR_LOG,
    DOWNLOAD_LOG,
    SET_TIME,
    
};
/*function prototype*/
void dash_board(void);       
void clear_log(void);
void download_log(void);
void set_time(void);
void store_event(void);
void menu_display(void);        
    
#endif	/* MAIN_H */

