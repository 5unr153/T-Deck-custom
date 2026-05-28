
#include "Arduino.h"
#include "ui_deckpro_port.h"
#include "factory.h"
#include "utilities.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <TinyGPS++.h>
#include "peripheral.h"
#include "WiFi.h"
#include <ctype.h>
#include <TouchDrvCSTXXX.hpp>


// extern 
extern TouchDrvCSTXXX touch;


volatile int default_language = DEFAULT_LANGUAGE_EN;
volatile bool default_keypad_light = false;
volatile bool default_motor_status = false;
volatile bool default_gps_status = true;
volatile bool default_lora_status = true;
volatile bool default_gyro_status = true;
volatile bool default_a7682_status = true;
// ----

void ui_disp_full_refr(void)
{
    disp_full_refr();
}
//************************************[ screen 0 ]****************************************** menu
//************************************[ screen 1 ]****************************************** lora

static float lora_default_freq = 850.0;
static int lora_default_band = 125;
static int lora_default_power = 22;

float ui_lora_get_freq(void) { return lora_default_freq; }
void ui_lora_set_freq(float freq) { lora_default_freq = freq; }
int ui_lora_get_bandwidth(void) { return lora_default_band; }
void ui_lora_set_bandwidth(float bd) { lora_default_band = bd; }
int ui_lora_get_power(void) { return lora_default_power; }
void ui_lora_set_power(float po) { lora_default_power = po; }

void ui_lora_param_set(void)
{
    lora_param_set();
}

int ui_lora_get_mode(void)
{
    return lora_get_mode();
}
void ui_lora_set_mode(int mode)
{
    lora_set_mode(mode);
}
void ui_lora_send(const char *str)
{
    lora_transmit(str);
}
void ui_lora_recv_loop(void)
{
    lora_receive_loop();
}
bool ui_lora_get_recv(const char **str, int *rssi)
{
    return lora_get_recv(str, rssi);
}
void ui_lora_set_recv_flag(void)
{
    lora_set_recv_flag();
}
//************************************[ screen 2 ]****************************************** setting
#if 1
// set function
// DEFAULT_LANGUAGE_CN、DEFAULT_LANGUAGE_EN
void ui_setting_set_language(int language)
{
    default_language = language;
}
void ui_setting_set_keypad_light(bool on)
{
    digitalWrite(BOARD_KEYBOARD_LED, on);
    default_keypad_light = on;
}
void ui_setting_set_motor_status(bool on)
{
    digitalWrite(BOARD_MOTOR_PIN, on);
    default_motor_status = on;
}
void ui_setting_set_gps_status(bool on)
{
    // enable GPS module power
    digitalWrite(BOARD_GPS_EN, on);
    default_gps_status = on;
}
void ui_setting_set_lora_status(bool on)
{
    // enable LORA module power
    digitalWrite(BOARD_LORA_EN, on);
    default_lora_status = on;
}
void ui_setting_set_gyro_status(bool on)
{
    // enable gyroscope module power
    digitalWrite(BOARD_1V8_EN, on);
    default_gyro_status = on;
}
void ui_setting_set_a7682_status(bool on)
{
    // enable 7682 module power
    digitalWrite(BOARD_6609_EN, on);
    digitalWrite(BOARD_A7682E_PWRKEY, on);
    default_a7682_status = on;
}

// get function
int ui_setting_get_language(void)
{
    return default_language;
}
bool ui_setting_get_keypad_light(void)
{
    return default_keypad_light;
}
bool ui_setting_get_motor_status(void)
{
    return default_motor_status;
}
bool ui_setting_get_gps_status(void)
{
    return default_gps_status;
}
bool ui_setting_get_lora_status(void)
{
    return default_lora_status;
}
bool ui_setting_get_gyro_status(void)
{
    return default_gyro_status;
}
bool ui_setting_get_a7682_status(void)
{
    return default_a7682_status;
}

// About System
const char *ui_setting_get_sf_ver(void)
{
    return UI_T_DECK_PRO_VERSION;
}
const char *ui_setting_get_hd_ver(void)
{
    return BOARD_T_DECK_PRO_VERSION;
}

void ui_setting_get_sd_capacity(uint64_t *total, uint64_t *used)
{
    if(ui_test_sd_card())
    {
        shared_spi_lock();
        shared_spi_prepare_device(BOARD_SD_CS);

        if(total)
            *total = SD.totalBytes() / (1024 * 1024);
        if(used)
            *used = SD.usedBytes() / (1024 * 1024);

        printf("total=%lluMB, used=%lluMB\n", *total, *used);

        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.printf("SD Card Size: %lluMB\n", cardSize);

        uint64_t totalSize = SD.totalBytes() / (1024 * 1024);
        Serial.printf("SD Card Total: %lluMB\n", totalSize);

        uint64_t usedSize = SD.usedBytes() / (1024 * 1024);
        Serial.printf("SD Card Used: %lluMB\n", usedSize);
        shared_spi_unlock();
    }
}

#endif
//************************************[ screen 3 ]****************************************** GPS
void ui_gps_task_suspend(void)
{
    gps_task_suspend();
}
void ui_gps_task_resume(void)
{
    gps_task_resume();
}
void ui_gps_get_coord(double *lat, double *lng)
{
    gps_get_coord(lat, lng);
}
void ui_gps_get_data(uint16_t *year, uint8_t *month, uint8_t *day)
{
    gps_get_data(year, month, day);
}
void ui_gps_get_time(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    gps_get_time(hour, minute, second);
}

void ui_gps_get_satellites(uint32_t *vsat)
{
    gps_get_satellites(vsat);
}
void ui_gps_get_speed(double *speed)
{
    gps_get_speed(speed);
}
//************************************[ screen 4 ]****************************************** Wifi Scan
int is_chinese_utf8(const char *str) {
    unsigned char c = (unsigned char)str[0];
    return (c >= 0xE0 && c <= 0xEF);  // 检查第一个字节是否在 UTF-8 的中文字符范围内
}

void ui_wifi_get_scan_info(ui_wifi_scan_info_t *list, int list_len)
{
    int n = WiFi.scanNetworks();
    if(n > list_len)
        n = list_len;
    
    memset(list, 0, (sizeof(*list) * list_len));
    for(int i = 0; i < n; i++)
    {
        const char *str = WiFi.SSID(i).c_str();
        if(is_chinese_utf8(str))
            continue;
        strncpy(list[i].name, WiFi.SSID(i).c_str(), 16);
        list[i].rssi = WiFi.RSSI(i);
    }
}
//************************************[ screen 5 ]****************************************** Test
bool ui_test_get(int peri_id)
{
    return peri_init_st[peri_id];
}
bool ui_test_sd_card(void) 
{
    return peri_init_st[E_PERI_SD];
}
bool ui_test_a7682e(void) 
{
    return peri_init_st[E_PERI_A7682E];
}
bool ui_test_pcm5102a(void)
{
    return peri_init_st[E_PERI_PCM5102A];
}

//************************************[ screen 6 ]****************************************** Battery
#if 1

// BQ25896
bool ui_battery_25896_is_vbus_in(void)
{
    return PPM.isVbusIn();
}

bool ui_batt_25896_is_chg(void)
{
    if(PPM.isCharging() == false) {
        return false;
    } else {
        return true;
    }
    // return true;
}
float ui_batt_25896_get_vbus(void)
{
    return (PPM.getVbusVoltage() *1.0 / 1000.0 );
    // return 4.5;
}
float ui_batt_25896_get_vsys(void)
{
    return (PPM.getSystemVoltage() * 1.0 / 1000.0);
    // return 4.5;
}
float ui_batt_25896_get_vbat(void)
{
    return (PPM.getBattVoltage() * 1.0 / 1000.0);
    // return 4.5;
}
float ui_batt_25896_get_volt_targ(void)
{
    return (PPM.getChargeTargetVoltage() * 1.0 / 1000.0);
    // return 4.5; 
}
float ui_batt_25896_get_chg_curr(void)
{
    return (PPM.getChargeCurrent());
    // return 4.5;
}
float ui_batt_25896_get_pre_curr(void)
{
    return (PPM.getPrechargeCurr());;
    // return 4.5;
}
const char * ui_batt_25896_get_chg_st(void)
{
    return PPM.getChargeStatusString();
    // return "hello";
}
const char * ui_batt_25896_get_vbus_st(void)
{
    return PPM.getBusStatusString();
    // return "hello";
}
const char * ui_batt_25896_get_ntc_st(void)
{
    return PPM.getNTCStatusString();
    // return "hello";
}
/* 27220 */
bool ui_battery_27220_is_vaild(void) {return peri_init_st[E_PERI_BQ27220]; }
bool ui_battery_is_external_power_present(void)
{
    if (peri_init_st[E_PERI_BQ25896]) {
        return PPM.isVbusIn();
    }
    if (peri_init_st[E_PERI_BQ27220]) {
        return bq27220.getAverageCurrent() > 0;
    }
    return false;
}
bool ui_battery_27220_get_input(void) { return ui_battery_is_external_power_present(); }
bool ui_battery_27220_get_charge_finish(void) { return bq27220.getCharingFinish();}
uint16_t ui_battery_27220_get_status(void) 
{
    BQ27220BatteryStatus batt;
    bq27220.getBatteryStatus(&batt);
    return batt.full;
}
uint16_t ui_battery_27220_get_voltage(void) { return bq27220.getVoltage(); }
int16_t ui_battery_27220_get_current(void) { return bq27220.getCurrent(); }
uint16_t ui_battery_27220_get_temperature(void) { return bq27220.getTemperature(); }
uint16_t ui_battery_27220_get_full_capacity(void) { return bq27220.getFullChargeCapacity(); }
uint16_t ui_battery_27220_get_design_capacity(void) { return bq27220.getDesignCapacity(); }
uint16_t ui_battery_27220_get_remain_capacity(void) { return bq27220.getRemainingCapacity(); }
uint16_t ui_battery_27220_get_percent(void) { return bq27220.getStateOfCharge(); }
uint16_t ui_battery_27220_get_health(void) { return bq27220.getStateOfHealth(); }
bool ui_battery_27220_is_low_alarm(void)
{
    if (!peri_init_st[E_PERI_BQ27220]) {
        return false;
    }

    BQ27220BatteryStatus batt = {0};
    BQ27220OperationStatus oper = {0};
    BQ27220GaugingStatus gauging = {0};

    bq27220.getBatteryStatus(&batt);
    bq27220.getGaugingStatus(&gauging);

    return batt.reg.SYSDWN || batt.reg.TDA || gauging.reg.EDV;
}
const char * ui_battert_27220_get_percent_level(void)
{
    int percent = bq27220.getStateOfCharge();
    const char * str = NULL;
    if(percent < 20)      str =  LV_SYMBOL_BATTERY_EMPTY;
    else if(percent < 40) str =  LV_SYMBOL_BATTERY_1;
    else if(percent < 65) str =  LV_SYMBOL_BATTERY_2;
    else if(percent < 90) str =  LV_SYMBOL_BATTERY_3;
    else                  str =  LV_SYMBOL_BATTERY_FULL;
    return str;
}
#endif
//************************************[ screen 7 ]****************************************** Input
int ui_input_get_touch_coord(int *x, int *y)
{
    int16_t last_x = 0;
    int16_t last_y = 0;
    // int ret = touch.getPoint(&last_x, &last_y);
    int ret = hyn_touch_get_point(&last_x, &last_y, 1);

    *x = last_x;
    *y = last_y;
    return ret;
}

int ui_input_get_keypay_val(char *v)
{
    return keypad_get_val(v);
}

void ui_input_set_keypay_flag(void)
{
    keypad_set_flag();
}

int ui_other_get_LTR(int *ch0, int *ch1, int *ps)
{
    // if(ch0 != NULL) *ch0 = lv_rand(0, LCD_VER_SIZE);
    // if(ch1 != NULL) *ch1 = lv_rand(0, LCD_VER_SIZE);
    // if(ps  != NULL) *ps  = lv_rand(0, LCD_VER_SIZE);

    if((ch0 != NULL) && (ch1 != NULL) && (ps != NULL))
    {
        *ch0 = LTR_553ALS_get_channel(0);
        *ch1 = LTR_553ALS_get_channel(1);
        *ps  = LTR_553ALS_get_ps();
    }
    else
    {
        Serial.printf("[%d] %s : Argument cannot be empty", __LINE__, __FILE__);
    }
    return 1;
}

int ui_other_get_gyro(float *gyro_x, float *gyro_y, float *gyro_z)
{
    // if(gyro_x != NULL) *gyro_x = lv_rand(0, LCD_VER_SIZE);
    // if(gyro_y != NULL) *gyro_y = lv_rand(0, LCD_VER_SIZE);
    // if(gyro_z != NULL) *gyro_z = lv_rand(0, LCD_VER_SIZE);

    if((gyro_x != NULL) && (gyro_x != NULL) && (gyro_x != NULL))
    {
        BHI260AP_get_val(2, gyro_x, gyro_y, gyro_z);
    }
    else
    {
        Serial.printf("[%d] %s : Argument cannot be empty", __LINE__, __FILE__);
    }
    return 1;
}

//************************************[ screen 8 ]****************************************** A7682E
bool ui_a7682_at_cb(const char *at_cmd)
{
    printf("[A7682E] at cmd: %s\n", at_cmd);

    modem.sendAT("+CTTSPARAM=1,3,0,1,1");

    delay(100);

    modem.sendAT("+CTTS=2,\"1234567890\"");

    return false;
}

void ui_a7682_call(const char *number)
{
    char buf[32];
    lv_snprintf(buf, 32, "D%s;", number);
    printf("[A7682E] at cmd: %s\n", buf);

    modem.sendAT(buf);
    delay(100);
}

void ui_a7682_hang_up(void)
{
    modem.sendAT("+CHUP");
    delay(100);
}

void ui_a7682_loop_resume(void)
{
    vTaskResume(a7682_handle);
}

void ui_a7682_loop_suspend(void)
{
    vTaskSuspend(a7682_handle);
}

//************************************[ screen 9 ]****************************************** Input

void ui_shutdown_on(void)
{
    ink_screen_prepare_shutdown();
    PPM.shutdown();
    Serial.println("Shutdown .....");
}

//************************************[ screen 10 ]****************************************** PCM5102
bool ui_pcm5102_cb(const char *at_cmd)
{
    audio.connecttoFS(SPIFFS, "/iphone_call.mp3");
    return true;
}

void ui_pcm5102_stop(void)
{
    audio.stopSong();
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void ui_sd_get_file_list(char **file_names, int max_files)
{
    if (!ui_test_sd_card()) {
        Serial.println("[SD] Card not mounted");
        return;
    }
    
    shared_spi_lock();
    shared_spi_prepare_device(BOARD_SD_CS);
    
    File root = SD.open("/");
    if (!root) {
        Serial.println("[SD] Failed to open root");
        shared_spi_unlock();
        return;
    }
    
    int count = 0;
    File file = root.openNextFile();
    while (file && count < max_files) {
        if (file_names[count]) {
            // Добавляем ведущий слеш
            snprintf(file_names[count], 64, "/%s", file.name());
            Serial.printf("[SD] Found: %s (%d bytes)\n", file_names[count], file.size());
        }
        file.close();
        file = root.openNextFile();
        count++;
    }
    
    root.close();
    shared_spi_unlock();
}

size_t ui_sd_get_file_size(const char *path)
{
    if (!path || !ui_test_sd_card()) return 0;
    
    shared_spi_lock();
    shared_spi_prepare_device(BOARD_SD_CS);
    
    File file = SD.open(path, FILE_READ);
    size_t size = file ? file.size() : 0;
    if (file) file.close();
    
    shared_spi_unlock();
    return size;
}

const char* ui_sd_read_file(const char *path)
{
    static char *buffer = NULL;
    static size_t buffer_size = 0;
    
    if (!path || !ui_test_sd_card()) {
        return "SD card not available";
    }
    
    shared_spi_lock();
    shared_spi_prepare_device(BOARD_SD_CS);
    
    File file = SD.open(path, FILE_READ);
    if (!file) {
        Serial.printf("[SD] Cannot open: %s\n", path);
        shared_spi_unlock();
        return "Cannot open file";
    }
    
    size_t file_size = file.size();
    Serial.printf("[SD] Opening: %s (%d bytes)\n", path, file_size);
    
    // Расширяем буфер если нужно
    if (file_size + 1 > buffer_size) {
        if (buffer) free(buffer);
        buffer_size = file_size + 1024; // Запас 1KB
        buffer = (char*)malloc(buffer_size);
        if (!buffer) {
            file.close();
            shared_spi_unlock();
            return "Out of memory";
        }
    }
    
    size_t bytes_read = file.readBytes(buffer, file_size);
    buffer[bytes_read] = '\0';
    
    file.close();
    shared_spi_unlock();
    
    if (bytes_read == 0) {
        return "File is empty";
    }
    
    return buffer;
}

// Глобальное состояние ридера
static sd_reader_state_t reader_state = {0};
static File current_file;
static char read_buffer[4096];  // Буфер для чтения (4KB)
static bool file_opened = false;
char reader_path[256];

// Имя файла для сохранения состояния (на SD)
#define READER_STATE_FILE ".reader_state.dat"

// Сохранить состояние на SD
bool sd_reader_save_state(void)
{
    if (!ui_test_sd_card()) return false;
    
    shared_spi_lock();
    shared_spi_prepare_device(BOARD_SD_CS);
    sprintf(reader_path, "%s%s", reader_state.file_path, READER_STATE_FILE);
    File state_file = SD.open(reader_path, FILE_WRITE);
    if (!state_file) {
        shared_spi_unlock();
        return false;
    }
    
    size_t written = state_file.write((uint8_t*)&reader_state, sizeof(reader_state));
    state_file.close();
    
    shared_spi_unlock();
    
    Serial.printf("[SD Reader] State saved: offset=%d, total=%d\n", 
                  reader_state.offset, reader_state.total_size);
    
    return (written == sizeof(reader_state));
}

// Загрузить состояние с SD
bool sd_reader_load_state(const char *path)
{
    if (!ui_test_sd_card()) return false;
    
    shared_spi_lock();
    shared_spi_prepare_device(BOARD_SD_CS);
    sprintf(reader_path, "%s%s", path, READER_STATE_FILE);
    File state_file = SD.open(reader_path, FILE_READ);
    if (!state_file) {
        shared_spi_unlock();
        return false;
    }
    
    sd_reader_state_t loaded_state;
    size_t read_bytes = state_file.read((uint8_t*)&loaded_state, sizeof(loaded_state));
    state_file.close();
    
    shared_spi_unlock();
    
    if (read_bytes != sizeof(loaded_state)) {
        return false;
    }
    
    // Проверяем, что это тот же файл
    if (strcmp(loaded_state.file_path, path) == 0) {
        memcpy(&reader_state, &loaded_state, sizeof(reader_state));
        Serial.printf("[SD Reader] State loaded: offset=%d, total=%d\n", 
                      reader_state.offset, reader_state.total_size);
        return true;
    }
    
    return false;
}

void sd_reader_set(int offset_mod){
    reader_state.offset += offset_mod;
    if (reader_state.offset < 0){
        reader_state.offset = 0;
    }
    current_file.seek(reader_state.offset);
    sd_reader_save_state();

}

// Открыть файл для резидентового чтения
bool sd_reader_open(const char *path)
{
    if (!path || !ui_test_sd_card()) {
        Serial.println("[SD Reader] Invalid path or no SD card");
        return false;
    }
    
    // Закрываем предыдущий файл если открыт
    if (file_opened) {
        sd_reader_close();
    }
    
    shared_spi_lock();
    shared_spi_prepare_device(BOARD_SD_CS);
    
    // Открываем файл
    current_file = SD.open(path, FILE_READ);
    if (!current_file) {
        Serial.printf("[SD Reader] Cannot open: %s\n", path);
        shared_spi_unlock();
        return false;
    }
    
    // Сохраняем информацию
    strncpy(reader_state.file_path, path, sizeof(reader_state.file_path) - 1);
    reader_state.total_size = current_file.size();
    reader_state.last_update = millis();
    
    // Пытаемся загрузить сохранённое состояние
    if (!sd_reader_load_state(path)) {
        // Если нет сохранённого состояния, начинаем с начала
        reader_state.offset = 0;
        reader_state.is_open = true;
        sd_reader_save_state();  // Сохраняем начальное состояние
    }
    
    // Перемещаемся на сохранённую позицию
    if (reader_state.offset > 0) {
        reader_state.offset -= 312;
        current_file.seek(reader_state.offset);
        Serial.printf("[SD Reader] Resumed at offset: %d\n", reader_state.offset);
    }
    
    reader_state.is_open = true;
    file_opened = true;
    
    shared_spi_unlock();
    
    Serial.printf("[SD Reader] Opened: %s, size=%d, resume at=%d\n", 
                  path, reader_state.total_size, reader_state.offset);
    
    return true;
}

// Закрыть файл (сохраняет состояние)
void sd_reader_close(void)
{
    if (!file_opened) return;
    
    shared_spi_lock();
    shared_spi_prepare_device(BOARD_SD_CS);
    
    if (current_file) {
        // Сохраняем финальное состояние перед закрытием
        reader_state.is_open = false;
        sd_reader_save_state();
        current_file.close();
    }
    
    file_opened = false;
    memset(&reader_state, 0, sizeof(reader_state));
    
    shared_spi_unlock();
    
    Serial.println("[SD Reader] Closed");
}

// Получить текущую позицию
size_t sd_reader_get_position(void)
{
    return reader_state.offset;
}

// Получить общий размер
size_t sd_reader_get_total(void)
{
    return reader_state.total_size;
}

// Проверить, достигнут ли конец файла
bool sd_reader_is_eof(void)
{
    return (file_opened && reader_state.offset >= reader_state.total_size);
}

// Сбросить прогресс чтения (начать сначала)
void sd_reader_reset(void)
{
    if (file_opened && current_file) {
        reader_state.offset = 0;
        current_file.seek(0);
        sd_reader_save_state();
        Serial.println("[SD Reader] Reset to beginning");
    }
}


// Чтение следующих N строк из файла
// Возвращает количество прочитанных строк
int sd_reader_read_lines(int max_lines, int max_chars, char *buffer, size_t buffer_size)
{
    if (!file_opened || !current_file) {
        return 0;
    }
    
    shared_spi_lock();
    shared_spi_prepare_device(BOARD_SD_CS);
    
    int lines_read = 0;
    size_t buffer_pos = 0;
    
    buffer[0] = '\0';
    
    while ( lines_read < max_lines && buffer_pos < max_chars && !sd_reader_is_eof()) {
        
        // Читаем один символ за раз 

        char c = current_file.read();
        reader_state.offset++;

        if (c == '\n') {
            lines_read++;  // Конец строки
        } 

        if (c != '\r') {
            buffer[buffer_pos++] = c;
        }

    }
    
    shared_spi_unlock();
    sd_reader_save_state();
    return lines_read;
}


// void audio_id3data(const char *info){  //id3 metadata
//     Serial.print("id3data     ");Serial.println(info);
// }
// void audio_eof_mp3(const char *info){  //end of file
//     Serial.print("eof_mp3     ");Serial.println(info);
// }
// void audio_showstation(const char *info){
//     Serial.print("station     ");Serial.println(info);
// }
// void audio_showstreamtitle(const char *info){
//     Serial.print("streamtitle ");Serial.println(info);
// }
// void audio_bitrate(const char *info){
//     Serial.print("bitrate     ");Serial.println(info);
// }
// void audio_commercial(const char *info){  //duration in sec
//     Serial.print("commercial  ");Serial.println(info);
// }
// void audio_icyurl(const char *info){  //homepage
//     Serial.print("icyurl      ");Serial.println(info);
// }
// void audio_lasthost(const char *info){  //stream URL played
//     Serial.print("lasthost    ");Serial.println(info);
// }
