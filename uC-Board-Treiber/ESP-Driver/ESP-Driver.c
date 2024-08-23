/*********************************************************************************\
*
* MMMMMMMMMMMM   SSSSSSSSSSSS   WW   WW   WW   MECHATRONIK
* MM   MM   MM   SS             WW   WW   WW   SCHULE
* MM   MM   MM   SSSSSSSSSSSS   WW   WW   WW   WINTERTHUR
* MM   MM   MM             SS   WW   WW   WW   
* MM   MM   MM   SSSSSSSSSSSS   WWWWWWWWWWWW   www.msw.ch
*
*
* Dateiname: ESP32_Board_Treiber.c
*
* Projekt  : ESP32 Board Treiber 
* Hardware : ESP32-S3
*
* Copyright: MSW, E4
*
* Beschreibung:
* =============
* Treiber für das ESP32 Board
*
* Portbelegung:
* =============
* Siehe Hardwarestruktur
*
* Verlauf:
* ========
* Datum:      Autor:         Version   Grund der Änderung:
* 23.08.2024  S. Huruvarshan V1.0      Neuerstellung
*
\*********************************************************************************/
#include "ESP-Driver.h"

#define INPUT_BTN0 21 
#define INPUT_BTN1 35 
#define INPUT_BTN2 36 
#define INPUT_BTN3 37

#define GPIO_INPUT_SEL ((1ULL << INPUT_BTN0) | (1ULL << INPUT_BTN1) | (1ULL << INPUT_BTN2) | (1ULL << INPUT_BTN3)); 

#define ADDR_IC2 0x20 
#define ADDR_IC3 0x21 
#define SCL 46 
#define SDA 47

void initI2C (void); 
void initMCP23017(uint8_t address);
void mcp23017WriteRegister(uint8_t address, uint8_t register, uint8_t data);

void initBoard(uint8_t startAnimation){
    //--------------------------------- GPIO Configuration ---------------------------------
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = GPIO_INPUT_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);  

    //--------------------------------- I2C and MCP23017 Configuration ---------------------------------
    initI2C (); 
    initMCP23017(ADDR_IC2);
    //initMCP23017(ADDR_IC3);
    mcp23017WriteRegister(ADDR_IC2, 0x00, 0x00);
    mcp23017WriteRegister(ADDR_IC2, 0x01, 0x00);
    mcp23017WriteRegister(ADDR_IC2, 0x14, 0xff);
    mcp23017WriteRegister(ADDR_IC2, 0x15, 0xff);
} 

void initI2C (void){ 
    int i2c_master_port = 0;
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
        .clk_flags = 0,
    };    
    i2c_param_config(I2C_NUM_0, &i2c_config);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0); 
}

void initMCP23017(uint8_t address){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x05, true);
    i2c_master_write_byte(cmd, 0x00, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
} 

void mcp23017WriteRegister(uint8_t address, uint8_t reg, uint8_t data){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true); 
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}