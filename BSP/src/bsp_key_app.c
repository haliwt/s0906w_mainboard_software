#include"bsp.h"


//uint8_t power_on_key_counter;
uint8_t mode_key_counter;
uint8_t complex_counter;

#define SHORT_PRESS_THRESHOLD   50
#define LONG_PRESS_THRESHOLD    60
#define COUNTER_LOCK            200

static void handle_power_key(void);
static void handle_mode_key(void);
static void handle_down_key(void);
static void handle_up_key(void);

/*
	*@brief: all key parase state
	*@notice:all key handler function
    #@param:
*/
void key_handler(void)
{
    if (g_key.key_power_flag == KEY_POWER_ID && g_key.key_down_flag != KEY_DOWN_ID &&
        g_key.key_up_flag   != KEY_UP_ID){
        handle_power_key();
    }
    else if (g_key.key_mode_flag == KEY_MODEL_ID)
    {
        handle_mode_key();
    }
    else if (g_key.key_down_flag == KEY_DOWN_ID && g_key.key_down_flag != 5)
    {
        handle_down_key();
    }
    else if (g_key.key_up_flag == KEY_UP_ID)
    {
        handle_up_key();
    }
}

/*
	*@brief:Power Key 
	*@notice:
    *@param:
    *@retrval
    
*/
static void handle_power_key(void)
{
    if (KEY_POWER_VALUE() == KEY_DOWN && g_pro.gpower_on == power_on && g_key.power_on_key_counter < 100)
    {
        g_key.power_on_key_counter++;
        if (g_key.power_on_key_counter > LONG_PRESS_THRESHOLD - 1)
        {
            g_key.power_on_key_counter = COUNTER_LOCK;
            g_key.key_long_power_flag = KEY_LONG_POWER;

            g_wifi.gTimer_wifi_led_fast_blink = 0;
            g_wifi.gwifi_link_net_state_flag = 0;
            g_wifi.wifi_led_fast_blink_flag = 1;
            g_wifi.link_net_step = 0;
            g_pro.first_connect_wifi_flag = 0;
            g_pro.set_temperature_success_flag = 0;

            buzzer_sound();
            g_pro.key_long_power_pressed = 1;
            g_pro.gTimer_key_long_counter = 0;

            if (g_disp.g_second_disp_flag == 1) {
                wifi_led_fast_blink_handler();
                SendData_Set_Command(CMD_CONNECT_WIFI, 0x01);
                osDelay(5);
                wifi_led_fast_blink_handler();
            }
        }
    }
    else if (KEY_POWER_VALUE() == KEY_UP && g_key.power_on_key_counter < LONG_PRESS_THRESHOLD)
    {
        g_key.key_power_flag = 0;
        g_key.power_on_key_counter = 0;
        buzzer_sound();

        g_pro.gpower_on = (g_pro.gpower_on == power_off) ? power_on : power_off;
        g_wifi.gwifi_normal_power_on_flag = 0;
        SendData_Set_Command(CMD_POWER,
                             (g_pro.gpower_on == power_on) ? open : close);
        osDelay(5);
    }
}

/*
	*@brief:mode Key 
	*@notice:
    *@param:
    *@retrval
    
*/
static void handle_mode_key(void)
{
    if (KEY_MODE_VALUE() == KEY_DOWN && mode_key_counter < 100) {
        mode_key_counter++;
        if (mode_key_counter > LONG_PRESS_THRESHOLD - 1) {
            mode_key_counter = COUNTER_LOCK;
            if (!g_pro.fan_warning && !g_pro.ptc_warning) {
                buzzer_sound();
                g_pro.key_gtime_timer_define_state = timer_time_mode;
                g_key.key_mode_long_flag = 1;
                g_pro.gTimer_switch_set_timer_times = 0;
				g_pro.gTimer_mainboard_fun_counter=0; //WT.EDIT 2025.11.07
                g_pro.gAI = 0;
                HUMIDITY_ICON_OFF();
                TEMP_ICON_OFF();
                TM1639_Display_3_Digit(g_pro.disp_59minutes_flag ? g_pro.disp_timer_minutes_value :g_pro.gdisp_timer_hours_value);
            }
        }
    }
    else if (KEY_MODE_VALUE() == KEY_UP) {
        if (mode_key_counter < SHORT_PRESS_THRESHOLD) {
            g_key.key_mode_flag++;
            mode_key_counter = 0;
            g_pro.gTimer_switch_set_timer_times = 0;
			g_pro.gTimer_mainboard_fun_counter=0; //WT.EDIT 2025.11.07
            if (!g_pro.fan_warning && !g_pro.ptc_warning) {
                buzzer_sound();
                // mode_short_key_fun();
            }
        } else if (mode_key_counter == COUNTER_LOCK) {
            g_key.key_mode_flag++;
            mode_key_counter = 0;
        }
    }
}

/* ---------------- Down Key ---------------- */
static void handle_down_key(void)
{
    if (KEY_DOWN_VALUE() == KEY_DOWN && complex_counter < 100) {
        complex_counter++;
        if (complex_counter > SHORT_PRESS_THRESHOLD - 1) {
            complex_counter = COUNTER_LOCK;
            buzzer_sound();
            g_pro.led_bar ^= 0x01;
            if (g_pro.led_bar)
                LED_TAPE_CTL_OFF();
            else
                LED_TAPE_CTL_ON();
        }
    }
    else if (KEY_DOWN_VALUE() == KEY_UP) {
        if (complex_counter < SHORT_PRESS_THRESHOLD) {
            g_key.key_down_flag = KEY_NULL;
            complex_counter = 0;
            if (!g_pro.fan_warning && !g_pro.ptc_warning) {
                buzzer_sound();
                key_dwon_fun();
            }
        } else if (complex_counter == COUNTER_LOCK) {
            complex_counter = 0;
            g_key.key_down_flag++;
        }
    }
}

/* ---------------- Up Key ---------------- */
static void handle_up_key(void)
{
    if (KEY_UP_VALUE() == KEY_UP) {
        g_key.key_up_flag = KEY_NULL;
        if (!g_pro.fan_warning && !g_pro.ptc_warning) {
            buzzer_sound();
            key_up_fun();
        }
    }
}









