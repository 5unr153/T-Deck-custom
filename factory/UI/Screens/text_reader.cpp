#include "UI/ui_deckpro.h"
#include "UI/ui_deckpro_port.h"
#include "sd_browser.h"

static lv_obj_t *reader_label;          // Label вместо textarea
static lv_obj_t *reader_status_label;
static lv_obj_t *reader_progress_bar;
static lv_obj_t *reader_page_label;
static lv_obj_t *reader_info_label;
static lv_timer_t *reader_timer;

// Буфер для текущей страницы
static char reader_page_buffer[MAX_CHARS + 100];

const char *selected_file_path;


static void reader_update_display(void)
{
    size_t pos = sd_reader_get_position();
    size_t total = sd_reader_get_total();
    int percent = (total > 0) ? (pos * 100 / total) : 0;
    
    // Обновляем статус
    if (reader_status_label) {
        lv_label_set_text_fmt(reader_status_label, "%d%% (%d/%d KB)", 
                              percent, pos / 1024, total / 1024);
    }
    

}

// Загрузить страницу (N строк)
static bool reader_load_current_page(void)
{
    if (sd_reader_is_eof()) {
        lv_label_set_text(reader_label, "--- END OF FILE ---");
        //ui_disp_full_refr();
        return false;
    }
    
    // Очищаем буфер
    memset(reader_page_buffer, 0, sizeof(reader_page_buffer));
    
    // Читаем строки
    int lines_read = sd_reader_read_lines(MAX_LINES_ON_SCREEN, MAX_CHARS,
                                           reader_page_buffer, 
                                           sizeof(reader_page_buffer));
    

    if (lines_read > 0) {
        lv_label_set_text(reader_label, reader_page_buffer);
        reader_update_display();
        //ui_disp_full_refr();
        return true;
    }

    return false;
}

// Загрузить следующую страницу
static void reader_next_page_cb(lv_event_t *e)
{
    if (sd_reader_is_eof()) {
        lv_label_set_text(reader_label, "--- END OF FILE ---");
        return;
    }
    
    // Загружаем следующую страницу
    reader_load_current_page();
}

static void reader_back_page_cb(lv_event_t *e)
{
    sd_reader_set(-MAX_CHARS*2);
    // Загружаем  страницу
    reader_load_current_page();
}

// Сбросить и начать сначала
static void reader_reset_cb(lv_event_t *e)
{
    sd_reader_reset();
    reader_load_current_page();
}

// Кнопка Back
static void reader_back_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_CLICKED) {
        sd_reader_close();
        scr_mgr_pop(false);
    }
}



static void reader_timer_event(lv_timer_t *t)
{
    static int sec = 0;
    char keypay_v;

    int ret = ui_input_get_keypay_val(&keypay_v);
    if(ret > 0)
    {
        ui_input_set_keypay_flag();
        if (keypay_v == 'p')
        {
            reader_load_current_page();
        }
                if (keypay_v == 'q')
        {
            sd_reader_set(-MAX_CHARS*2);
            reader_load_current_page();
        }
        sec = 0;
    }

    sec++;
    if(sec > 60) // 2s
    {
        sec = 0;
    }
}

static void create_reader(lv_obj_t *parent) 
{
   
    // Статус (проценты)
    reader_status_label = lv_label_create(parent);
    lv_obj_align(reader_status_label, LV_ALIGN_TOP_RIGHT, 0, 10);
    lv_obj_set_style_text_font(reader_status_label, FONT_BOLD_SIZE_14, LV_PART_MAIN);
    
    // Label для отображения текста (вместо textarea)
    reader_label = lv_label_create(parent);
    lv_obj_set_width(reader_label, LV_HOR_RES - 10);
    lv_obj_set_height(reader_label, LV_VER_RES - 60);
    lv_obj_align(reader_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_label_set_long_mode(reader_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(reader_label, FONT_BOLD_SIZE_14, LV_PART_MAIN);
    lv_obj_set_style_pad_all(reader_label, 5, LV_PART_MAIN);
    
    
    // Кнопка Back
    selected_file_path = sd_browser_get_selected_file();
    scr_back_btn_create(parent, selected_file_path, reader_back_cb);
    
    // Панель управления внизу
    lv_obj_t *controls = lv_obj_create(parent);
    lv_obj_set_size(controls, LV_HOR_RES, 40);
    lv_obj_align(controls, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_border_width(controls, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(controls, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(controls, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(controls, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(controls, LV_OBJ_FLAG_SCROLLABLE);
    
    // Кнопка Reset
    lv_obj_t *reset_btn = lv_btn_create(controls);
    lv_obj_set_size(reset_btn, 65, 35);
    lv_obj_t *reset_label = lv_label_create(reset_btn);
    lv_label_set_text(reset_label, "Reset");
    lv_obj_center(reset_label);
    lv_obj_add_event_cb(reset_btn, reader_reset_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *back_btn = lv_btn_create(controls);
    lv_obj_set_size(back_btn, 65, 35);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "<< Back");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, reader_back_page_cb, LV_EVENT_CLICKED, NULL);


    // Кнопка Next
    lv_obj_t *next_btn = lv_btn_create(controls);
    lv_obj_set_size(next_btn, 65, 35);
    lv_obj_t *next_label = lv_label_create(next_btn);
    lv_label_set_text(next_label, "Next >>");
    lv_obj_center(next_label);
    lv_obj_add_event_cb(next_btn, reader_next_page_cb, LV_EVENT_CLICKED, NULL);
}

static void entry_reader(void) 
{
    selected_file_path = sd_browser_get_selected_file();

    reader_timer = lv_timer_create(reader_timer_event, 50, NULL);
    if (!selected_file_path) {
        lv_label_set_text(reader_label, "No file selected.\nGo back and select a file.");
        ui_disp_full_refr();
        return;
    }
    
    // Открываем файл
    if (!sd_reader_open(selected_file_path)) {
        lv_label_set_text(reader_label, "Failed to open file.\n\n"
                           "Check:\n- File exists\n- SD card inserted");
        ui_disp_full_refr();
        return;
    }
    
    // Загружаем первую страницу
    reader_load_current_page();
}

static void exit_reader(void){ 
    sd_reader_close();
    ui_disp_full_refr();
    if(reader_timer)
    {
        lv_timer_del(reader_timer);
        reader_timer = NULL;
    }
    ui_disp_full_refr();
}

static void destroy_reader(void) 
{
    // Очистка
}

scr_lifecycle_t screen_reader = {
    .create = create_reader,
    .entry = entry_reader,
    .exit  = exit_reader,
    .destroy = destroy_reader,
};