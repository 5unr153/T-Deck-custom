#include "UI/ui_deckpro.h"
#include "UI/ui_deckpro_port.h"

#include <TouchDrvCSTXXX.hpp>


static void sleep_timer_event(lv_timer_t* t)
{
     extern TouchDrvCSTXXX touch;
    touch.sleep();

    lora_sleep();

    SerialGPS.end();
    
    // pinMode(BOARD_GPS_PPS, OUTPUT);
    // pinMode(BOARD_GPS_RXD, OUTPUT);
    // pinMode(BOARD_GPS_TXD, OUTPUT);
    // pinMode(BOARD_LORA_RST, OUTPUT);
    // pinMode(BOARD_TOUCH_RST, OUTPUT);
    // pinMode(BOARD_LORA_BUSY, OUTPUT);

    // digitalWrite(BOARD_GPS_PPS, LOW);
    // digitalWrite(BOARD_GPS_RXD, LOW);
    // digitalWrite(BOARD_GPS_TXD, LOW);
    // digitalWrite(BOARD_LORA_RST, LOW);
    // digitalWrite(BOARD_TOUCH_RST, LOW);
    // digitalWrite(BOARD_LORA_BUSY, LOW);

    gpio_reset_pin((gpio_num_t)BOARD_GPS_PPS);
    gpio_reset_pin((gpio_num_t)BOARD_GPS_RXD);
    gpio_reset_pin((gpio_num_t)BOARD_GPS_TXD);
    gpio_reset_pin((gpio_num_t)BOARD_LORA_RST);
    gpio_reset_pin((gpio_num_t)BOARD_TOUCH_RST);
    gpio_reset_pin((gpio_num_t)BOARD_LORA_BUSY);

    digitalWrite(BOARD_6609_EN, LOW);
    digitalWrite(BOARD_LORA_EN, LOW);
    digitalWrite(BOARD_GPS_EN, LOW);
    
    digitalWrite(BOARD_1V8_EN, LOW);
    digitalWrite(BOARD_A7682E_PWRKEY, LOW);

    // gpio_hold_en((gpio_num_t)BOARD_GPS_PPS);
    // gpio_hold_en((gpio_num_t)BOARD_TOUCH_RST);
    // gpio_hold_en((gpio_num_t)BOARD_GPS_RXD);
    // gpio_hold_en((gpio_num_t)BOARD_GPS_TXD);
    // gpio_hold_en((gpio_num_t)BOARD_LORA_RST);
    // gpio_hold_en((gpio_num_t)BOARD_LORA_BUSY);
    gpio_hold_en((gpio_num_t)BOARD_6609_EN);
    gpio_hold_en((gpio_num_t)BOARD_LORA_EN);
    gpio_hold_en((gpio_num_t)BOARD_GPS_EN);
    gpio_hold_en((gpio_num_t)BOARD_1V8_EN);
    gpio_hold_en((gpio_num_t)BOARD_A7682E_PWRKEY);
    gpio_deep_sleep_hold_en();

    
    // esp_sleep_enable_ext0_wakeup((gpio_num_t)ENCODER_KEY, 0);                            
    esp_sleep_enable_ext1_wakeup((1UL << BOARD_BOOT_PIN), ESP_EXT1_WAKEUP_ANY_LOW);   // Hibernate using user keys
    esp_deep_sleep_start();
    lv_timer_del(t);
}

static void create11(lv_obj_t *parent)
{
   
    lv_obj_t * img = lv_img_create(parent);
    lv_img_set_src(img, &img_win);
    lv_obj_center(img);
    

    // back 
   lv_timer_create(sleep_timer_event, 2000, (void *)parent);
}
static void entry11(void) 
{
    ui_disp_full_refr();
}
static void exit11(void) {
    ui_disp_full_refr();
}
static void destroy11(void) { }

scr_lifecycle_t screen11 = {
    .create = create11,
    .entry = entry11,
    .exit  = exit11,
    .destroy = destroy11,
};