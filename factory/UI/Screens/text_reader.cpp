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
        if (keypay_v == 'S')
        {
            reader_load_current_page();
        }
        if (keypay_v == 'b')
        {
            sd_reader_set(-MAX_CHARS*2);
            reader_load_current_page();
        }
        if (keypay_v == 'r')
        {
            sd_reader_reset();
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
    lv_obj_set_height(reader_label, LV_VER_RES - 30);
    lv_obj_align(reader_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_label_set_long_mode(reader_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_font(reader_label, FONT_BOLD_SIZE_14, LV_PART_MAIN);
    lv_obj_set_style_pad_all(reader_label, 5, LV_PART_MAIN);
    
    
    // Кнопка Back
    selected_file_path = sd_browser_get_selected_file();
    scr_back_btn_create(parent, selected_file_path, reader_back_cb);
    

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