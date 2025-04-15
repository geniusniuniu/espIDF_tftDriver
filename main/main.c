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
    tft_fill_screen(0x0000);
    //画一个点
    tft_draw_point(125,125,0x0000);  
    //画一条斜线      
    tft_draw_line(100, 150, 100, 150, 0xffff); 
    //画一个矩形
    tft_draw_rectangle(x_start,y_start,x_end, y_end, 0x07E0,TFT_NOT_FILLED);
    //画一个填充矩形
    tft_draw_rectangle(x_start,y_start+80,x_end, y_end+80, 0xffff,TFT_IS_FILLED);
    //画圆
    tft_draw_circle(100, 100, 50, 0xffff,TFT_IS_FILLED);
    
}