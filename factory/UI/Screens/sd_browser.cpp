#include "UI/ui_deckpro.h"
#include "UI/ui_deckpro_port.h"


static lv_obj_t *scr7_list;
static lv_obj_t *scr7_status_label;

static const char *selected = nullptr;

void sd_browser_set_selected_file(const char *path) {
    selected = path;
}

const char* sd_browser_get_selected_file(void) {
    return selected;
}

static void scr7_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        scr_mgr_pop(false);
    }
}

static void scr7_file_click_cb(lv_event_t *e)
{
    char *path = (char*)lv_event_get_user_data(e);
    if (!path) return;
    
    // Сохраняем путь в глобальную переменную для следующего экрана
    // (или передаём через user_data при push)
    sd_browser_set_selected_file(path);
    // Переходим на экран просмотра
    scr_mgr_push(SCREEN7_1_ID, false);
}

static void scr7_refresh_list(lv_obj_t *parent)
{
    // Очищаем список
    while(lv_obj_get_child_cnt(scr7_list) > 0) {
        lv_obj_del(lv_obj_get_child(scr7_list, 0));
    }
    
    lv_label_set_text(scr7_status_label, "Scanning SD card...");
    ui_disp_full_refr();
    
    // Получаем список файлов
    char *file_names[30] = {0};
    char buffers[30][64];
    for(int i = 0; i < 30; i++) {
        buffers[i][0] = '\0';
        file_names[i] = buffers[i];
    }
    
    ui_sd_get_file_list(file_names, 30);
    
    // Создаём кнопки для каждого файла
    int file_count = 0;
    for(int i = 0; i < 30; i++) {
        if (file_names[i][0] == '\0') break;
        file_count++;
             // Получаем размер файла для отображения
        size_t fsize = ui_sd_get_file_size(file_names[i]);

        // Формируем строку: имя файла + размер
        char display_name[80];
        snprintf(display_name, 80, "%s", file_names[i] + 1);
        
        lv_obj_t *btn = lv_list_add_btn(scr7_list, LV_SYMBOL_FILE, display_name);
        lv_obj_set_height(btn, 25);
        lv_obj_set_style_text_font(btn, FONT_BOLD_SIZE_14, LV_PART_MAIN);
        
        // Сохраняем полный путь
        char *path_copy = (char*)lv_mem_alloc(strlen(file_names[i]) + 1);
        strcpy(path_copy, file_names[i]);
        lv_obj_add_event_cb(btn, scr7_file_click_cb, LV_EVENT_CLICKED, path_copy);
       
    }
    
    lv_label_set_text_fmt(scr7_status_label, "%d files", file_count);
    ui_disp_full_refr();
}

static void create7(lv_obj_t *parent) 
{
    
    // Статусная строка
    scr7_status_label = lv_label_create(parent);
    lv_label_set_text(scr7_status_label, "Initializing...");
    lv_obj_set_style_text_font(scr7_status_label, FONT_BOLD_SIZE_14, LV_PART_MAIN);
    lv_obj_align(scr7_status_label, LV_ALIGN_TOP_LEFT, 10, 35);
    
    // Список файлов
    scr7_list = lv_list_create(parent);
    lv_obj_set_size(scr7_list, LV_HOR_RES - 10, lv_pct(70));
    lv_obj_align(scr7_list, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_style_bg_color(scr7_list, DECKPRO_COLOR_BG, LV_PART_MAIN);
    lv_obj_set_style_pad_top(scr7_list, 5, LV_PART_MAIN);
    lv_obj_set_style_pad_row(scr7_list, 3, LV_PART_MAIN);
    
    lv_obj_t *back7_label = scr_back_btn_create(parent, ("SD Card"), scr7_btn_event_cb);

}

static void entry7(void) 
{
    // Обновляем список ОДИН РАЗ при входе
    scr7_refresh_list(lv_scr_act());
    ui_disp_full_refr();
}

static void exit7(void) {
    ui_disp_full_refr();
}

static void destroy7(void) { }

scr_lifecycle_t screen_browser = {
    .create = create7,
    .entry = entry7,
    .exit  = exit7,
    .destroy = destroy7,
};