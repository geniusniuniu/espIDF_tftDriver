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

void tft_draw_point(uint16_t x, uint16_t y, uint16_t color)  
{  
    x += ili9341->x_gap;  
    y += ili9341->y_gap;  
    // 确保坐标在显示范围内  
    assert((x < LCD_H_RES ) && (y< LCD_V_RES) && " position must in 240*320 ");

    // 定义一个1x1的位图  
    uint16_t color_data = color ; // 假设是16位颜色，深度可能不同，需根据实际显示面板位深调整  

    esp_lcd_panel_io_tx_param(ili9341->io, LCD_CMD_CASET, (uint8_t[]) { (x >> 8) & 0xFF, x & 0xFF,  (x >> 8) & 0xFF, x & 0xFF}, 4);  
    esp_lcd_panel_io_tx_param(ili9341->io, LCD_CMD_RASET, (uint8_t[]) { (y >> 8) & 0xFF, y & 0xFF,  (y >> 8) & 0xFF, y & 0xFF}, 4);  

    size_t len = sizeof(uint16_t);  
    esp_lcd_panel_io_tx_color(ili9341->io, LCD_CMD_RAMWR, &color_data, len);  
}  

//填充整个屏幕
void tft_fill_screen(uint16_t color)
{
    esp_lcd_panel_io_tx_param(ili9341->io, LCD_CMD_CASET, (uint8_t[]) { (0x00>>8)&0xff, 0x00&0xff, ((0xF0-1)>>8) & 0xFF, 0xEF & 0xFF}, 4);  
    esp_lcd_panel_io_tx_param(ili9341->io, LCD_CMD_RASET, (uint8_t[]) { (0x00>>8)&0xff, 0x00&0xff, ((0x140-1)>>8)& 0xFF, (0x140-1) & 0xFF }, 4);  

    uint16_t *color_data = malloc(LCD_H_RES * LCD_V_RES * 2); // 假设RGB565每像素2字节 
    memset(color_data, color, LCD_H_RES * LCD_V_RES * 2);                     //填充颜色数据
    esp_lcd_panel_io_tx_color(ili9341->io, LCD_CMD_RAMWR, color_data, LCD_H_RES * LCD_V_RES * 2);

    ESP_LOGI(TAG, "Filled Screen  OK!");
}

void tft_draw_line(uint16_t x_start, uint16_t x_end,uint16_t y_start ,uint16_t y_end,uint16_t color)
{
    uint8_t yflag = 0, xyflag = 0;
    int delta_x ;
    int delta_y ;
    int incrE ;
    int incrNE ;
    int d;  //初始误差
    int x;
    int y;

    x_start += ili9341->x_gap;  
    x_end += ili9341->x_gap;
    y_start+= ili9341->y_gap;  
    y_end += ili9341->y_gap;
    
    // 确保坐标在显示范围内  
    assert((x_start < LCD_H_RES && x_end < LCD_H_RES) && (y_start < LCD_V_RES && y_end < LCD_V_RES) && " position must in 240*320 ");

    //确定画线方向
    if (x_end - x_start == 0)   //垂直线
    {
        if(y_end < y_start){xy_SWAP(&y_start,&y_end);}  //交换首尾坐标
        for(; y_start < y_end; y_start++)
        {
            tft_draw_point(x_start, y_start, color);
        }
    } else if (y_end - y_start == 0)    //水平线
    {
        if(x_end < x_start){xy_SWAP(&x_start,&x_end);}  //交换首尾坐标
        for(;x_start < x_end; x_start++)
        {
            tft_draw_point(x_start, y_start, color);
        }
    }
    else    // 画斜线 
    {
        if(x_start > x_end)
        {
            /*交换两点坐标*/
			/*交换后不影响画线，但是画线方向由第一、二、三、四象限变为第一、四象限*/
            xy_SWAP(&x_start,&x_end);
            xy_SWAP(&y_start,&y_end);
        }  
        if(y_start > y_end)
        {
            /*将Y坐标取负*/
			/*取负后影响画线，但是画线方向由第一、四象限变为第一象限*/
            y_start = -y_start;
            y_end = -y_end;
            /*置标志位yflag，记住当前变换，在后续实际画线时，再将坐标换回来*/
            yflag = 1;
        }
        if(y_end -y_start > x_end - x_start)    //直线斜率大于1
        {
            /*将X坐标与Y坐标互换*/
			/*互换后影响画线，但是画线方向由第一象限0~90度范围变为第一象限0~45度范围*/
            xy_SWAP(&x_start,&y_start);
            xy_SWAP(&x_end,&y_end);
            /*置标志位xyflag，记住当前变换，在后续实际画线时，再将坐标换回来*/
            xyflag = 1;
        }

        /*使用Bresenham算法画直线*/
		/*算法要求，画线方向必须为第一象限0~45度范围*/
        delta_x = x_end - x_start;
        delta_y = y_end - y_start;
        incrE = 2 * delta_y;
		incrNE = 2 * (delta_y - delta_x);
        d = 2*delta_y - delta_x;  //初始误差
        x = x_start;
        y = y_start;

        /*画起始点，同时判断标志位，将坐标换回来*/
        if(yflag && xyflag)            tft_draw_point(y, -x, color);
        else if(yflag)                      tft_draw_point(x, -y, color);
        else if(!yflag && xyflag)   tft_draw_point(y, x, color);
        else                                    tft_draw_point(x, y, color);

        while(x < x_end)
        {    
            x++;
            if(d < 0)   //选择E
            {
                d += incrE;
            }
            else    //选择NE
            {
                y++;
                d += incrNE;
            }
            /*画点，同时判断标志位，将坐标换回来*/
            if(yflag && xyflag)            tft_draw_point(y, -x, color);
            else if(yflag)                      tft_draw_point(x, -y, color);
            else if(!yflag && xyflag)   tft_draw_point(y, x, color);
            else                                    tft_draw_point(x, y, color);
        }
    }
    ESP_LOGI(TAG, "Draw Line OK!");     
}  

//画矩形
void tft_draw_rectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color,uint8_t filled)
{
    if(filled == TFT_NOT_FILLED)   //不填充
   {
         tft_draw_line(x_start, x_end, y_start, y_start, color);    //上边
         tft_draw_line(x_start, x_end, y_end, y_end, color);       //下边
         tft_draw_line(x_start, x_start, y_start, y_end, color);    //左边
         tft_draw_line(x_end, x_end, y_start, y_end, color);       //右边
   }
   else
   {
        for(int i = x_start; i < x_end; i++)
        {
            for(int j = y_start; j < y_end; j++)
            {
                tft_draw_point(i, j, color);
            }
        }
   }
    ESP_LOGI(TAG, "Draw Rectangle OK!"); 
}

//画圆
void tft_draw_circle(uint16_t A, uint16_t B, uint16_t r, uint16_t color,uint8_t filled)
{
    int d = 1-r;
    int X=0;
    int Y=r;
    A += ili9341->x_gap;
    B += ili9341->y_gap;

    // 确保坐标在显示范围内
    assert((A+r < LCD_H_RES) && (B+r < LCD_V_RES) && " position must in 240*320 ");
    assert((A-r > 0) && (B-r > 0) && " position must in 240*320 ");

    //先画出x，y轴上的点
    //x轴正方向
    tft_draw_point(A+0, B+r, color);
    //x轴负方向
    tft_draw_point(A-0, B-r, color);
    //y轴正方向
    tft_draw_point(A+Y, B+0, color);
    //y轴负方向
    tft_draw_point(A-Y, B-0, color);

    //填充圆心列
    if(filled == TFT_IS_FILLED)
    {
        tft_draw_line(A,A,B-Y,B+Y,color);
    }


    //在第一象限遍历每个点
    while (X < Y)
    {
        X++;
        if(d<0) //东方衍生新像素点
        {
            d += 2*X+1;
        }
        else    //东南方衍生新像素点
        {
            Y--;
            d += 2*X-2*Y+1;
        }
        //画出这个点对应的8个点
        tft_draw_point(A+X, B+Y, color); 
        tft_draw_point(A+Y, B+X, color); 
        tft_draw_point(A-X, B-Y,  color); 
        tft_draw_point(A-Y, B-X,  color);
        tft_draw_point(A+X, B-Y, color);
        tft_draw_point(A+Y, B-X, color);
        tft_draw_point(A-X, B+Y, color);
        tft_draw_point(A-Y, B+X, color);

        if(filled == TFT_IS_FILLED)   //填充
        {
            //由圆心向两侧填充每一列
            for(int i = -Y; i < Y; i++)
            {
                tft_draw_point(A+X, B+i, color);
                tft_draw_point(A-X, B+i, color);
            }

            //填充两侧
            for(int j = -X; j < X; j++)
            {
                tft_draw_point(A-Y, B+j, color);
                tft_draw_point(A+Y, B+j, color);
            }
        }
    }
    ESP_LOGI(TAG, "Draw Circle OK!");
}
