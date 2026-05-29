#include "UI/ui_deckpro.h"
#include "UI/ui_deckpro_port.h"

lv_obj_t * scr4_list;
static lv_obj_t *scr4_lab_buf[20];

static void scr4_list_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    for(int i = 0; i < lv_obj_get_child_cnt(obj); i++) 
    {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        if(lv_obj_check_type(child, &lv_label_class)) {
            char *str = lv_label_get_text(child);

            if(strcmp("- WIFI Config", str) == 0)
            {
                scr_mgr_push(SCREEN4_1_ID, false);
            }
            if(strcmp("- WIFI Scan", str) == 0)
            {
                scr_mgr_push(SCREEN4_2_ID, false);
            }
            printf("%s\n", str);
        }
    }
}

static void scr4_item_create(const char *name, lv_event_cb_t cb)
{
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_list_btn_class, scr4_list);
    lv_obj_class_init_obj(obj);
    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);

    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, name);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_set_height(obj, LV_VER_RES / 6);
    lv_obj_set_style_text_font(obj, FONT_BOLD_SIZE_15, LV_PART_MAIN);
    // lv_obj_set_style_bg_color(obj, lv_color_hex(EPD_COLOR_BG), LV_PART_MAIN);
    // lv_obj_set_style_text_color(obj, lv_color_hex(EPD_COLOR_FG), LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_outline_width(obj, 1, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(obj, cb, LV_EVENT_CLICKED, NULL); 
}


static void create4(lv_obj_t *parent) 
{
    scr4_list = lv_list_create(parent);
    lv_obj_set_size(scr4_list, lv_pct(93), lv_pct(91));
    lv_obj_align(scr4_list, LV_ALIGN_BOTTOM_MID, 0, 0);
    // lv_obj_set_style_bg_color(scr4_list, lv_color_hex(EPD_COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_pad_top(scr4_list, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_row(scr4_list, 15, LV_PART_MAIN);
    lv_obj_set_style_radius(scr4_list, 0, LV_PART_MAIN);
    // lv_obj_set_style_outline_pad(scr4_list, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(scr4_list, 0, LV_PART_MAIN);
    // lv_obj_set_style_border_color(scr4_list, lv_color_hex(EPD_COLOR_FG), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(scr4_list, 0, LV_PART_MAIN);

    scr4_item_create("- WIFI Config", scr4_list_event);
    scr4_item_create("- WIFI Scan", scr4_list_event);

    // back
    scr_back_btn_create(parent, "WIFI", scr_btn_event_cb);
}

static void entry4(void) 
{
    ui_disp_full_refr();
}
static void exit4(void) {
    ui_disp_full_refr();
}
static void destroy4(void) { }

scr_lifecycle_t screen4 = {
    .create = create4,
    .entry = entry4,
    .exit  = exit4,
    .destroy = destroy4,
};

// --------------------- screen 4.2 --------------------- Wifi Config


static void create4_1(lv_obj_t *parent) 
{


    // back
    scr_back_btn_create(parent, "Wifi Config", scr_btn_event_cb);
}

static void entry4_1(void) 
{
    ui_disp_full_refr();
}
static void exit4_1(void) {
    ui_disp_full_refr();
}
static void destroy4_1(void) { }

scr_lifecycle_t screen4_1 = {
    .create = create4_1,
    .entry = entry4_1,
    .exit  = exit4_1,
    .destroy = destroy4_1,
};

// --------------------- screen 4.2 --------------------- Wifi Scan

static lv_obj_t *scr4_2_cont;
static lv_obj_t *wifi_scan_lab;
static lv_timer_t *wifi_scan_timer = NULL;

static ui_wifi_scan_info_t wifi_info_list[UI_WIFI_SCAN_ITEM_MAX];

static void show_wifi_scan(void)
{
#define BUFF_LEN 400
    char buf[BUFF_LEN];
    int ret = 0, offs = 0;

    ret = lv_snprintf(buf + offs, BUFF_LEN, "       NAME      | RSSI\n");
    offs = offs + ret;
    ret = lv_snprintf(buf + offs, BUFF_LEN, "-----------------------\n");
    offs = offs + ret;

    for(int i = 0; i < UI_WIFI_SCAN_ITEM_MAX; i++) {
        if(strcmp(wifi_info_list[i].name, "") == 0 && wifi_info_list[i].rssi == 0)
        {
            break;
        }
        if(i == UI_WIFI_SCAN_ITEM_MAX - 1) {
            ret = lv_snprintf(buf + offs, BUFF_LEN, "%-16.16s | %4d", wifi_info_list[i].name, wifi_info_list[i].rssi);
            break;
        }

        ret = lv_snprintf(buf + offs, BUFF_LEN, "%-16.16s | %4d\n", wifi_info_list[i].name, wifi_info_list[i].rssi);
        offs = offs + ret;
    }
    lv_label_set_text(wifi_scan_lab, buf);
#undef BUFF_LEN
}

static void wifi_scan_timer_event(lv_timer_t *t)
{
    ui_wifi_get_scan_info(wifi_info_list, UI_WIFI_SCAN_ITEM_MAX);
    show_wifi_scan();
}

static void create4_2(lv_obj_t *parent) 
{
    scr4_2_cont = lv_obj_create(parent);
    lv_obj_set_size(scr4_2_cont, lv_pct(100), lv_pct(90));
    lv_obj_set_style_bg_color(scr4_2_cont, DECKPRO_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr4_2_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(scr4_2_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(scr4_2_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr4_2_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(scr4_2_cont, 13, LV_PART_MAIN);
    lv_obj_set_flex_flow(scr4_2_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(scr4_2_cont, 5, LV_PART_MAIN);
    lv_obj_set_style_pad_column(scr4_2_cont, 5, LV_PART_MAIN);
    lv_obj_set_align(scr4_2_cont, LV_ALIGN_BOTTOM_MID);

    wifi_scan_lab = lv_label_create(scr4_2_cont);
    lv_obj_set_width(wifi_scan_lab, lv_pct(95));
    lv_obj_set_style_pad_all(wifi_scan_lab, 0, LV_PART_MAIN);
    lv_obj_set_style_text_font(wifi_scan_lab, FONT_BOLD_MONO_SIZE_15, LV_PART_MAIN);   
    lv_obj_set_style_border_width(wifi_scan_lab, 0, LV_PART_MAIN);
    lv_label_set_long_mode(wifi_scan_lab, LV_LABEL_LONG_WRAP);

    lv_obj_t *back4_label = scr_back_btn_create(parent, ("Wifi"), scr_btn_event_cb);
}
static void entry4_2(void) 
{
    ui_disp_full_refr();
    wifi_scan_timer = lv_timer_create(wifi_scan_timer_event, 10000, NULL);
    lv_timer_ready(wifi_scan_timer);
}
static void exit4_2(void) {
    ui_disp_full_refr();
    if(wifi_scan_timer) {
        lv_timer_del(wifi_scan_timer);
        wifi_scan_timer = NULL;
    }
}

static void destroy4_2(void) { }

scr_lifecycle_t screen4_2 = {
    .create = create4_2,
    .entry = entry4_2,
    .exit  = exit4_2,
    .destroy = destroy4_2,
};