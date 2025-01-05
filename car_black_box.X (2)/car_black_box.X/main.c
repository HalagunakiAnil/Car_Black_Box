/* Name : Anil A Halagunaki
   Date :04/01/2025
   Project Name :Car Black Box 
 */

#include <xc.h>
#include "clcd.h"
#include "mkp.h"
#include "adc.h"
#include "eeprom.h"
#include "main.h"
#include "uart.h"
#include "i2c.h"
#include "ds1307.h"
#include "external_eeprom.h"


/* Declaring veriables Globale */
char time[9];
unsigned int spd, index;
unsigned static int store_idx;
unsigned char clock_reg[3];


unsigned char *str[] = {"ON", "GN", "GR", "G1", "G2", "G3", "G4", "-c"};
unsigned char *menu[] = {"VIEW LOG       ", "CLEAR LOG      ", "DOWNLOAD LOG   ", "SET TIME       "};
unsigned static int addre = 0x00;
unsigned static int event_count;
unsigned int flag;
unsigned char key;
unsigned int disp_flag = 0, disp_count;
unsigned int star_flag = 0;
unsigned int store_add = 0x00;
unsigned char store_arr[10][15];
unsigned int shift_idx;
unsigned int t_wait = 0;

static void init_config(void) 
{
    init_clcd();
    init_adc();
    init_mkp();
    init_uart();
    init_i2c();
    init_ds1307();
    
}

/* Function definition for getting real time */
static void get_time(void) 
{
    clock_reg[0] = read_ds1307(HOUR_ADDR);
    clock_reg[1] = read_ds1307(MIN_ADDR);
    clock_reg[2] = read_ds1307(SEC_ADDR);

    if (clock_reg[0] & 0x40) 
    {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    }
    else 
    {
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    }
    time[2] = ':';
    time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
    time[4] = '0' + (clock_reg[1] & 0x0F);
    time[5] = ':';
    time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
    time[7] = '0' + (clock_reg[2] & 0x0F);
    time[8] = '\0';
}

/* dashboard function definition*/
void dash_board(void)
{
    clcd_print("TIME     EV   SP", LINE1(0));
    clcd_print(time, LINE2(0));



    spd = read_adc(CHANNEL4) / 10.33;              //for display speed
    clcd_putch(spd / 10 + 48, LINE2(14));
    clcd_putch(spd % 10 + 48, LINE2(15));

    key = read_matrix_keypad(STATE);
    if (key == SW2)                      //switch 2 for incrementing gear
    {
        if (index == 7)
        {
            index = 1;                 //after collision come back to gear nutral
        }
        else if (index < 6)
        {
            index++;             //incrementing gear
            store_event();        //function call for storing 
        }
    }
    else if (key == SW3)
    {
        if (index == 7)               //switch 3 for decrementing gear
        {
            index = 1;                 //after collision come back to gear nutral
        }
        if (index > 1) 
        {
            index--;
            store_event();            //storing event
        }
    }
    else if (key == SW1)             //switch 1 for display the collision
    {
        index = 7;                   
        store_event();              //storing event
    }
    clcd_print(str[index], LINE2(9));
    /* 
     if (key == SW4)  //entering into the main menu
        {
             
                
                    static int a = 0x00;
                       CLEAR_DISP_SCREEN;
                       clcd_print("STORED DATA     ",LINE1(0));
                      char ch[11];
                    for(int i=0;i<10;i++)
                    {
                        ch[i] = read_internal_eeprom(a++);
                    }
                      ch[10] = '\0';
                       clcd_print(ch,LINE2(0));
                       while(1);
                       for(unsigned long int wait = 100000;wait--;);
        }
     */
}

/* function definition for display the stored  log*/
void view_log(void) 
{

    unsigned int val;
    static unsigned int wait = 0;
    if (event_count == 0)               //when no loges are present
    {
        wait++;
        if (wait > 0 && wait <= 1000) 
        {
            clcd_print("No Logs... ", LINE1(0));
            clcd_print("To view ", LINE2(0));
        }
        if (wait == 1000)
        {                        //setting the flag for come back to main menu
            flag = 1;
            disp_flag = 0;
        }
    }
    else 
    {
        clcd_print("# TIME    EV  SP", LINE1(0));     //when logs are present display the logs      
        for (int i = 0; i < event_count; i++)
        {
            val = (shift_idx + i) % 10;              //formula for getting index of stored event 
            store_add = val * 10;                 //formula for getting old event to new event
            store_arr[i][0] = read_external_eeprom(store_add++);   //reading events from external eeprom
            store_arr[i][1] = read_external_eeprom(store_add++);
            store_arr[i][2] = ':';
            store_arr[i][3] = read_external_eeprom(store_add++);
            store_arr[i][4] = read_external_eeprom(store_add++);
            store_arr[i][5] = ':';
            store_arr[i][6] = read_external_eeprom(store_add++);
            store_arr[i][7] = read_external_eeprom(store_add++);
            store_arr[i][8] = ' ';
            store_arr[i][9] = read_external_eeprom(store_add++);
            store_arr[i][10] = read_external_eeprom(store_add++);
            store_arr[i][11] = ' ';
            store_arr[i][12] = read_external_eeprom(store_add++);
            store_arr[i][13] = read_external_eeprom(store_add++);
            store_arr[i][14] = '\0';

        }
        if (key == SW2 && store_idx < event_count - 1) //switch 2 for scrolling down the events
        {
            store_idx++;
        } else if (key == SW1 && store_idx > 0)          //switch 1 for scrolling up the events
        {
            store_idx--;
        }

        clcd_putch(store_idx + 48, LINE2(0));               //displaying the events in clcd
        clcd_putch(' ', LINE2(1));
        clcd_print(store_arr[store_idx], LINE2(2));
    }

}

void clear_log(void) 
{
    static unsigned int delay = 0;
    delay++;
    if (delay > 0 && delay <= 1000)        //display the message when logs are cleared
    {
        clcd_print("LOGS ARE CLEARED", LINE1(0));
    }
    if (delay == 1000) 
    {                               //setting flags for came back to main menu
        CLEAR_DISP_SCREEN;
        event_count = 0;
        addre = 0;
        disp_count = 0;
        disp_flag = 0;
        store_idx = 0;
        flag = 1;
        delay = 0;
    }

}

void download_log(void) 
{
    unsigned int delay = 0;
    if (event_count == 0)
    { //when the no logs are present
        CLEAR_DISP_SCREEN;
        clcd_print("NO logs...", LINE1(0));             //displaying message in clcd
        clcd_print("To Download", LINE2(0));
        for (unsigned long int wait = 500000; wait--;);
        CLEAR_DISP_SCREEN;
        flag = 1;
        disp_flag = 0;
    } 
    else 
    {
        CLEAR_DISP_SCREEN;
        clcd_print("Downloading log", LINE1(0));      //message for when logs displaying in Tera term
        clcd_print("Through UART", LINE2(0));
        for (unsigned long int wait = 500000; wait--;);  //delay for displaying the message
        puts("TIME     EV  SP\n\r");
        for (int i = 0; i < event_count; i++)
        {
            puts(store_arr[i]);  //writing logs in to the tera term
            puts("\n\r");
        }
        for (unsigned long int wait = 500000; wait--;);
    }
    flag = 1;    //setting flags for after display the logs goes back to main menu
    disp_flag = 0;
    disp_count = 0;

}

void set_time(void) 
{
    static unsigned int field = 0;
    static unsigned int set_flag = 0;
    static unsigned int hr = 0, min = 0, sec = 0;
    clcd_print("HH MM SS", LINE1(0));
//    key = read_matrix_keypad(STATE);
    if(set_flag == 0 )
   {
     hr = ((time[0] - 48)* 10) + (time[1] - 48);    //assigning hour to variable
     min = ((time[3] - 48)* 10) + (time[4] - 48);   //assigning min to variable
     sec = ((time[6] - 48)* 10) + (time[7] - 48);   //assigning sec to variable
     set_flag = 1;
   }
    if (key == SW2) //switch 2 for changing field
    {
        field = (field + 1) % 3; //formula for changing field
    }

    if (key == SW1) //switch 1 key for incrementing
    {
        if (field == 0)
        {
            if (hr++ == 23) //incrementing time 
            {
                hr = 0;         //when Hour reach 59 assigning Zero
            }
        } 
        else if (field == 1) 
        {
            if (min++ == 59) //incrementing min
            {
                min = 0;              //when min reach 59 assigning Zero
            }
        } 
        else if (field == 2)
        {
            if (sec++ == 59) //incrementing sec
            {
                sec = 0;             //when sec reach 59 assigning Zero
            }
        }
    }
   
     // Blinking logic
    if (t_wait++ < 600)
    {
        clcd_putch((hr / 10) + '0', LINE2(0));
        clcd_putch((hr % 10) + '0', LINE2(1));
        clcd_putch(':', LINE2(2));
        clcd_putch((min / 10) + '0', LINE2(3));
        clcd_putch((min % 10) + '0', LINE2(4));
        clcd_putch(':', LINE2(5));
        clcd_putch((sec / 10) + '0', LINE2(6));
        clcd_putch((sec % 10) + '0', LINE2(7));
    }
else if (t_wait++ < 1000)
    {
        
    
        // Clear the selected field to create a blinking effect
        if (field == 0)
        {
            clcd_putch(' ', LINE2(0));
            clcd_putch(' ', LINE2(1));
        }
        else if (field == 1)
        {
            clcd_putch(' ', LINE2(3));
            clcd_putch(' ', LINE2(4));
        }
        else if (field == 2)
        {
            clcd_putch(' ', LINE2(6));
            clcd_putch(' ', LINE2(7));
        }
}
else {
    t_wait = 0;
    
}
    
     if (key == SW4)            //exit and save time
    {
        write_ds1307(HOUR_ADDR, (hr / 10) << 4 | (hr % 10)); //Assigning Real time 
        write_ds1307(MIN_ADDR, (min / 10) << 4 | (min % 10)); 
        write_ds1307(SEC_ADDR, (sec / 10) << 4 | (sec % 10)); 
        flag = 0;          //Go back to main menu
        
    } 
     else if (key == SW5)     //exit  without saving time
    {
        flag = 0;
    }
}

/* Function definition for Menu Display*/
void menu_display(void) 
{
    /* for handling the star*/
    if (disp_flag == 0)
    {
        clcd_putch('*', LINE1(0));
        clcd_putch(' ', LINE2(0));
    } 
    else if (disp_flag == 1) 
    {
        clcd_putch(' ', LINE1(0));
        clcd_putch('*', LINE2(0));
    }
    clcd_print(menu[disp_count], LINE1(1));
    clcd_print(menu[disp_count + 1], LINE2(1));

    /* for scroll down the menu*/
    if (key == SW2 && disp_count <= 2)
    {
        if (disp_flag == 0)
        {
            disp_flag = 1;

        } 
        else if (disp_count < 2) 
        {
            disp_count++;              //up scrolling the menu

        }
    }        /* for scroll up the menu */
    else if (key == SW1 && disp_count >= 0)
    {
        if (disp_flag == 1) 
        {
            disp_flag = 0;
        }
        else if (disp_count > 0) 
        {
            disp_count--;       //down scrolling the menu

        }
    }

}

void store_event(void) 
{
    write_external_eeprom(addre++, time[0]);     //storing the events to the external eeprom
    write_external_eeprom(addre++, time[1]);
    write_external_eeprom(addre++, time[3]);
    write_external_eeprom(addre++, time[4]);
    write_external_eeprom(addre++, time[6]);
    write_external_eeprom(addre++, time[7]);

    write_external_eeprom(addre++, str[index][0]);
    write_external_eeprom(addre++, str[index][1]);
    write_external_eeprom(addre++, (spd / 10) + 48);
    write_external_eeprom(addre++, (spd % 10) + 48);

    if (addre >= 100) //for storing the 100 bytes of data
        addre = 0;

    if (event_count < 10) 
    {
        event_count++; //storing 10 events after the 10 events it over write 1st event 
    } 
    else 
    {
        shift_idx = (shift_idx + 1) % 10;     
    }

}

void main(void) 
{
    init_config();

    while (1) 
    {
        get_time();         //function call for getting real time
        key = read_matrix_keypad(STATE);

        if (flag == DASH_BOARD)                 //for dash board
        {
            dash_board();              //switch 4 for entering particular log
            if (key == SW4) 
            {
                flag = 1;             //for entering main menu
            }
        } 
        else if (flag == MAIN_MENU)
        {
            menu_display();
            if (key == SW4)                  //for enter the view log
            {
                if (disp_count == 0 && disp_flag == 0)
                {
                    CLEAR_DISP_SCREEN;
                    flag = 2;                 //for entering the view log
                }
                else if ((disp_count == 0 && disp_flag == 1) || (disp_count == 1 && disp_flag == 0)) 
                {
                    CLEAR_DISP_SCREEN;
                    flag = 3;                    //for entering the clear log
                }
                else if ((disp_count == 1 && disp_flag == 1) || (disp_count == 2 && disp_flag == 0))
                {
                    CLEAR_DISP_SCREEN;
                    flag = 4; //for entering download log
                } 
                else if (disp_count == 2 && disp_flag == 1) 
                {
                    CLEAR_DISP_SCREEN;
                    flag = 5;           //for entering set time
                }

            } 
            else if (key == SW5)
            {                        //switch 5 for exiting the particular log
                CLEAR_DISP_SCREEN;
                flag = 0;
                disp_count = 0;

            }


        } 
        else if (flag == VIEW_LOG) 
        {

            view_log();
            if (key == SW5)  //exiting from the view log
            {
                flag = 1;
            }

        } 
        else if (flag == CLEAR_LOG) 
        {
            clear_log(); //for clear log
        }
        else if (flag == DOWNLOAD_LOG) 
        {
            view_log();
            download_log(); //for down load log
        } 
        else if (flag == SET_TIME) 
        {
            set_time(); //for set time
        }

    }
    
}