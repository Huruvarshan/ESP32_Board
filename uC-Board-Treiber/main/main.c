#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h> 
#include <driver/gpio.h> 
#include <led_strip.h>
//#include <mcp23x17.h>

#define OUTPUT_WS2812 38 

#define INPUT_BTN0 21 
#define INPUT_BTN1 35 
#define INPUT_BTN2 36 
#define INPUT_BTN3 37

#define GPIO_INPUT_SEL ((1ULL << INPUT_BTN0) | (1ULL << INPUT_BTN1) | (1ULL << INPUT_BTN2) | (1ULL << INPUT_BTN3)); 

#define ADDR_IC2 0x20 
#define ADDR_IC3 0x21 
#define SCL 46 
#define SDA 47

void app_main(void)
{ 
    //--------------------------------- GPIO Configuration ---------------------------------
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_INPUT_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf); 


    //--------------------------------- LED Strip Configuration ---------------------------------
    led_strip_handle_t led_strip;

    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = OUTPUT_WS2812,            // The GPIO that connected to the LED strip's data line
        .max_leds = 2,                              // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,   // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,              // LED strip model
        .flags.invert_out = false,                  // whether to invert the output signal (useful when your hardware has a level inverter)
    };

    led_strip_rmt_config_t rmt_config = {
    #if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
        .rmt_channel = 0,
    #else
        .clk_src = RMT_CLK_SRC_DEFAULT,             // different clock source can lead to different power consumption
        .resolution_hz = 10 * 1000 * 1000,          // 10MHz
        .flags.with_dma = false,                    // whether to enable the DMA feature
    #endif
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip)); 

    //--------------------------------- IO-Extender Configuration ---------------------------------
    //static mcp23x17_t dev = 0; 

    
    
    while (1)
    {
        if (gpio_get_level(INPUT_BTN0) | gpio_get_level(INPUT_BTN1) | gpio_get_level(INPUT_BTN2) | gpio_get_level(INPUT_BTN3))
        {
            printf("Button pressed\n");
            led_strip_set_pixel(led_strip, 0, 0, 50, 0); 
            led_strip_set_pixel(led_strip, 1, 0, 50, 0); 
            led_strip_refresh(led_strip);
        } else {
            led_strip_set_pixel(led_strip, 0, 50, 0, 0); 
            led_strip_set_pixel(led_strip, 1, 50, 0, 0); 
            led_strip_refresh(led_strip);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
