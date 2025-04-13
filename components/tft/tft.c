#include <stdio.h>
#include "tft.h"
#include <assert.h>

static const char *TAG = "TFT";

ili9341_color_t const ILI9341_BLACK          = (ili9341_color_t)0x0000;
ili9341_color_t const ILI9341_NAVY          = (ili9341_color_t)0x000F;
ili9341_color_t const ILI9341_DARKGREEN    = (ili9341_color_t)0x03E0;
ili9341_color_t const ILI9341_DARKCYAN     = (ili9341_color_t)0x03EF;
ili9341_color_t const ILI9341_MAROON     = (ili9341_color_t)0x7800;
ili9341_color_t const ILI9341_PURPLE     = (ili9341_color_t)0x780F;
ili9341_color_t const ILI9341_OLIVE     = (ili9341_color_t)0x7BE0;
ili9341_color_t const ILI9341_LIGHTGREY    = (ili9341_color_t)0xC618;
ili9341_color_t const ILI9341_DARKGREY     = (ili9341_color_t)0x7BEF;
ili9341_color_t const ILI9341_BLUE     = (ili9341_color_t)0x001F;
ili9341_color_t const ILI9341_GREEN     = (ili9341_color_t)0x07E0;
ili9341_color_t const ILI9341_CYAN     = (ili9341_color_t)0x07FF;
ili9341_color_t const ILI9341_RED     = (ili9341_color_t)0xF800;
ili9341_color_t const ILI9341_MAGENTA     = (ili9341_color_t)0xF81F;
ili9341_color_t const ILI9341_YELLOW     = (ili9341_color_t)0xFFE0;
ili9341_color_t const ILI9341_WHITE     = (ili9341_color_t)0xFFFF;
ili9341_color_t const ILI9341_ORANGE     = (ili9341_color_t)0xFD20;
ili9341_color_t const ILI9341_GREENYELLOW  = (ili9341_color_t)0xAFE5;
ili9341_color_t const ILI9341_PINK     = (ili9341_color_t)0xF81F;

 esp_lcd_panel_handle_t panel_handle = NULL;

void tft_Init(void)
{
	    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_SCLK,
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .quadwp_io_num = -1,        //禁用四线模式
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_LCD_DC,
        .cs_gpio_num = PIN_NUM_LCD_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS ,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = NULL,
        .user_ctx = NULL,
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };

    ESP_LOGI(TAG, "Install ILI9341 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, false));

    // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);
}

esp_err_t tft_draw_point(uint16_t x, uint16_t y, uint16_t color)  
{  
    x += ili9341->x_gap;  
    y += ili9341->y_gap;  

    // 确保坐标在显示范围内  
    assert((x < LCD_H_RES ) && (y< LCD_V_RES) && " position must in 240*320 ");

    // 定义一个1x1的位图  
    uint8_t color_data = (uint8_t)(color >> 8); // 假设是16位颜色，深度可能不同，需根据实际显示面板位深调整  

    esp_lcd_panel_io_tx_param(ili9341->io, LCD_CMD_CASET, (uint8_t[]) { (x >> 8) & 0xFF,  x & 0xFF,   (x >> 8) & 0xFF, x & 0xFF,  }, 4);  

    esp_lcd_panel_io_tx_param(ili9341->io, LCD_CMD_RASET, (uint8_t[]) { (y >> 8) & 0xFF,y & 0xFF,  (y >> 8) & 0xFF, y & 0xFF, }, 4);  

    size_t len = ili9341->fb_bits_per_pixel / 8;  
    esp_lcd_panel_io_tx_color(ili9341->io, LCD_CMD_RAMWR, &color_data, len);  

    return ESP_OK;  
}  