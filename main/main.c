#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "tft.h"

static const char *TAG = "main";


int x_start = 100;
int x_end   = 150;
int y_start = 100;
int y_end   = 150;

void app_main(void)
{
    tft_Init();
    ESP_LOGI(TAG, "LCD INIT OVER !");
    // 分配颜色数据缓冲区  
    uint16_t *color_data = malloc((x_end - x_start) * (y_end - y_start) * 2); // 假设RGB565每像素2字节  
    memset(color_data, 0x07E0, (x_end - x_start) * (y_end - y_start) * 2); 
    //画一个矩形
    panel_ili9341_draw_bitmap(x_start,y_start,x_end, y_end, color_data);
    
    free(color_data); 

}