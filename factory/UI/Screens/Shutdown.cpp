#include "UI/ui_deckpro.h"
#include "UI/ui_deckpro_port.h"

static lv_timer_t *shutdown_timer = NULL;


static void shutdown_timer_event(lv_timer_t* t)
{
    ui_shutdown_on();
    lv_timer_del(t);
}

static void create9(lv_obj_t *parent)
{
    if(ui_battery_25896_is_vbus_in()) 
    {
        lv_obj_t * label = lv_label_create(parent);
        lv_obj_set_width(label, lv_pct(95));
        lv_obj_set_style_text_font(label, FONT_BOLD_SIZE_15, LV_PART_MAIN);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_label_set_text(label, "The shutdown function can only be used when the "
                            "battery is connected alone, and cannot be shut down when connected to USB.");
        lv_obj_center(label);

        // back 
        scr_back_btn_create(parent, "Shoutdown", scr_btn_event_cb);
    } 
    else 
    {
        lv_obj_t * img = lv_img_create(parent);
        lv_img_set_src(img, &img_start);
        lv_obj_center(img);

        lv_timer_create(shutdown_timer_event, 2000, (void *)parent);
    }
}
static void entry9(void) 
{
    ui_disp_full_refr();
}
static void exit9(void) {
    ui_disp_full_refr();
}
static void destroy9(void) { }

scr_lifecycle_t screen9 = {
    .create = create9,
    .entry = entry9,
    .exit  = exit9,
    .destroy = destroy9,
};