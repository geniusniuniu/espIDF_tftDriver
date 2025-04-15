#ifndef  _TFT_H
#define _TFT_H

#include <stdint.h>
#include <stdio.h>

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ili9341.h"

// Using SPI2 in the example
#define LCD_HOST                             SPI2_HOST
#define TOUCH_HOST                       SPI3_HOST

#define LCD_PIXEL_CLOCK_HZ     (40 * 1000 * 1000) // Clock out at 80 MHz
#define LCD_BK_LIGHT_ON_LEVEL       1
#define LCD_BK_LIGHT_OFF_LEVEL      0

#define PIN_NUM_SCLK                         3
#define PIN_NUM_MOSI                        45
#define PIN_NUM_MISO                        46
//通过DC引脚的高低电平来直接指示当前传输的内容是命令还是数据，简化通信协议
#define PIN_NUM_LCD_DC                    47   

#define PIN_NUM_LCD_RST                   21
#define PIN_NUM_LCD_CS                     14
#define EXAMPLE_PIN_NUM_BK_LIGHT                 0


// #define EXAMPLE_PIN_NUM_TOUCH_CS          1
// #define EXAMPLE_PIN_NUM_TOUCH_SCLK      42
// #define EXAMPLE_PIN_NUM_TOUCH_MOSI     2
// #define EXAMPLE_PIN_NUM_TOUCH_MISO     41

#define LCD_H_RES              240
#define LCD_V_RES               320

// Bit number used to represent command and parameter
#define LCD_CMD_BITS            8
#define LCD_PARAM_BITS       8

#define xy_SWAP(a,b) 	do{int temp = *(a);*(a) = *(b); *(b) = temp;}while(0)

#define TFT_NOT_FILLED	0
#define TFT_IS_FILLED		1

 extern esp_lcd_panel_handle_t panel_handle;

typedef uint16_t ili9341_color_t; 

extern ili9341_color_t const ILI9341_BLACK ;
extern ili9341_color_t const ILI9341_NAVY;
extern ili9341_color_t const ILI9341_DARKGREEN;
extern ili9341_color_t const ILI9341_DARKCYAN;
extern ili9341_color_t const ILI9341_MAROON;
extern ili9341_color_t const ILI9341_PURPLE;
extern ili9341_color_t const ILI9341_OLIVE;
extern ili9341_color_t const ILI9341_LIGHTGREY;
extern ili9341_color_t const ILI9341_DARKGREY;
extern ili9341_color_t const ILI9341_BLUE;
extern ili9341_color_t const ILI9341_GREEN;
extern ili9341_color_t const ILI9341_CYAN;
extern ili9341_color_t const ILI9341_RED;
extern ili9341_color_t const ILI9341_MAGENTA ;
extern ili9341_color_t const ILI9341_YELLOW;
extern ili9341_color_t const ILI9341_WHITE;
extern ili9341_color_t const ILI9341_ORANGE ;
extern ili9341_color_t const ILI9341_GREENYELLOW;
extern ili9341_color_t const ILI9341_PINK;

void tft_Init(void);
void tft_fill_screen(uint16_t color);
void tft_draw_point(uint16_t x, uint16_t y, uint16_t color);  
void tft_draw_line(uint16_t x_start, uint16_t x_end,uint16_t y_start ,uint16_t y_end,uint16_t color);
void tft_draw_rectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color,uint8_t filled);
void tft_draw_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color,uint8_t filled);



#endif
