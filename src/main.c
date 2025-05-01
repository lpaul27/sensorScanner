#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor/w1_sensor.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

void w1_search_callback(struct w1_rom rom, void *user_data);
void ds18b20_request_temperatures(const struct device *w1);
int ds18b20_get_temp(const struct device *w1, uint64_t rom, float *temp);

#define DS18B20_CMD_CONVERT_TEMP 0x44
#define KNOWN_SENSORS 100

// Interval of data collection
K_TIMER_DEFINE(collection_interval, NULL, NULL);

// ID's of the temperature sensors
uint64_t known_roms[KNOWN_SENSORS] = {
        // list of all known sensors in order
        0x280305f90c0000cc, //1 
        0x28d676f90c0000c0, //2 
        0x285c05f90c0000c6, //3
        0x285a9ef90c0000b3, //4
        0x28423df90c000034, //5
        0x28f861f90c000048, //6
        0x28bd1cf90c000057, //7
        0x280842f90c00008c, //8
        0x283f7ef90c000041, //9
        0x28fd6cf90c00004f, //10
        0x28b748fa0c00009a, //11
        0x286e1af90c000056, //12
        0x28110ef90c000089, //13
        0x28839bf90c000033, //14
        0x285f86f90c00001a, //15
        0x281096f90c000037, //16
        0x28336cfa0c00004b, //17
        0x289b27fa0c00002f, //18
        0x283d81f90c0000e6, //19
        0x28d114fa0c00005f, //20
        0x28d98af90c000063, //21* 4/17/2025 
        0x288c83f90c000055, //22
        0x284d7df90c0000f9, //23
        0x28482cf90c00004a, //24
        0x28f14df90c000007, //25
        0x285930f90c00007b, //26
        0x280e38f90c0000ee, //27
        0x283333f90c000039, //28
        0x288e48fa0c0000e1, //29
        0x28858ef90c00002f, //30
        0x2894ddf90c000098, //31
        0x28e995f90c00009d, //32
        0x28a642f90c0000c3, //33
        0x28553ef90c0000a4, //34
        0x282c63f90c0000d3, //35
        0x281e93f90c0000f6, //36
        0x28f202f90c0000d8, //37
        0x28e9bff90c0000d8, //38
        0x287717fa0c0000ff, //39
        0x28316bfa0c000074, //40
        0x28e118f90c00001b, //41
        0x28699cf90c000084, //42
        0x282ccef90c00000c, //43
        0x2874adf90c0000dc, //44
        0x28efd0f90c000014, //45
        0x288ed2f90c000063, //46
        0x286af3f90c0000a3, //47
        0x28e627f90c000075, //48
        0x28971dfa0c00006b, //49
        0x281568fa0c000050, //50
        0x28466afa0c0000a4, //51
        0x28ff64f90c00001f, //52
        0x289c8ef90c0000e2, //53
        0x287361fa0c0000d2, //54
        0x28eee1f90c00006a, //55
        0x280859f90c000080, //56
        0x281826fa0c000051, //57
        0x28a338f90c0000f8, //58
        0x285d72fa0c000045, //59
        0x288873fa0c0000a7, //60
        0x28cc17f90c000088, //61
        0x28463afa0c000031, //62
        0x28bd49f90c000010, //63
        0x282c75f90c000033, //64
        0x286263fa0c00003d, //65
        0x28cbddf90c000092, //66
        0x285095f90c00000c, //67
        0x28d92af90c000050, //68
        0x2844fdf90c0000a4, //69
        0x28b7a0f90c0000f6, //70
        0x28b119fa0c000070, //71
        0x287548f90c0000e3, //72
        0x281462fa0c0000da, //73
        0x28d669f90c0000d3, //74
        0x283e39f90c0000ce, //75
        0x280ed6f90c000096, //76
        0x28f22ff90c0000cc, //77
        0x286354f90c000057, //78
        0x287197f90c00000e, //79
        0x284d9af90c000072, //80
        0x28d363fa0c00000d, //81
        0x28281cf90c00000d, //82
        0x287b60fa0c0000be, //83
        0x288e5efa0c000001, //84
        0x28343df90c00001e, //85
        0x288d96f90c0000cc, //86
        0x285696f90c0000f0, //87
        0x286b82f90c000034, //88
        0x28421af90c00009d, //89
        0x282b7df90c000088, //90
        0x286e4bf90c00000e, //91
        0x280203f90c000067, //92
        0x28fc02f90c0000cb, //93
        0x28ab25fa0c000041, //94
        0x28d624fa0c00005e, //95
        0x28de1af90c000051, //96
        0x286bfef80c0000f7, //97
        0x284245fa0c0000ef, //98
        0x28a2cff90c000038, //99
        0x28a719fa0c000099, //100
};

// Number of the temperature sensor wrt ID
uint32_t known_ids[KNOWN_SENSORS] = {
        // List of the number of known sensors 
        // in accordance to the ID's above
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        30,
        31,
        32,
        33,
        34,
        35,
        36,
        37,
        38,
        39,
        40,
        41,
        42,
        43,
        44,
        45,
        46,
        47,
        48,
        49,
        50,
        51,
        52,
        53,
        54,
        55,
        56,
        57,
        58,
        59,
        60,
        61,
        62,
        63,
        64,
        65,
        66,
        67,
        68,
        69,
        70,
        71,
        72,
        73,
        74,
        75,
        76,
        77,
        78,
        79,
        80,
        81,
        82,
        83,
        84,
        85,
        86,
        87,
        88,
        89,
        90,
        91,
        92,
        93,
        94,
        95,
        96,
        97,
        98,
        99,
        100,
};

// boolean to represent whether or not the sensor is working
bool present[KNOWN_SENSORS] = {false, false};

// Array to store the temperature data
float temp_readings[KNOWN_SENSORS];

// Leave level INF if running program. Only change to DBG for debug purposes
// program will not run correctly if left on DBG
LOG_MODULE_REGISTER(W1_Read_Multi, LOG_LEVEL_DBG);

static const struct device *w1 = DEVICE_DT_GET(DT_NODELABEL(w1_0));

// static const struct device *get_ds18b20_device(void){
//         const struct device *const dev = DEVICE_DT_GET_ANY(maxim_ds18b20);

//         if(dev == NULL){
//                 // no node found or does not have status 'okay'
//                 printk("\nError: no device found.\n");
//                 return NULL;
//         }

//         if(!device_is_ready(dev)){
//                 printk("\nError: Device \"%s\" is not ready; "
//                         "check the driver initialization logs for errors. \n",
//                         dev->name);
//                 return NULL;
//         }

//         printk("Found device \"%s\", getting sensor data\n", dev->name);
//         printk("test, \"%s\"", dev ->name);

//         if (dev->name == "280305f90c0000cc") {
//                 printk("Found 1\n");
//                 tempSensors[0] = &dev;
//         }
//         return dev;
// } // end get function


 int main(void){
        // stores temperature data
        float temp;

        if(!device_is_ready(w1)) {
                LOG_ERR("W1 device not ready");
                return -1;
        }

        LOG_DBG("Searching for 1-wire sensors");

        // used for identifying unknown sensor
        // important if a sensir needs to be replaced
        // causes error unused-variable, works fine.
        int num_sensors = w1_search_rom(w1, w1_search_callback, NULL);

        // Timer for data collection interval of 2 seconds
        k_timer_start(&collection_interval, K_MSEC(2000), K_MSEC(2000));

        // infinite loop for attaining temperature data
        while(true){
                
                if(k_timer_status_get(&collection_interval) >0){
                        ds18b20_request_temperatures(w1);
                        int sensor_num = 0;
                        for(int i = 0; i < KNOWN_SENSORS; i++){
                                temp = -666; // Used to indicate no temp reading
                                if(present[i]){
                                        int result = ds18b20_get_temp(w1, known_roms[i], &temp);
                                        sensor_num++;
                                        // if there was an error attaining the temperature or talking to sensor
                                        if(result < 0){
                                                LOG_ERR("Error reading temperature from sensor %d: %d", known_ids[i], result);
                                        }
                                }
                                k_msleep(9);
                                printk("%.2f\t", temp);

                        }

                        //print data from the for loop in one line string for the python code to read
                        printk("\n");
                        LOG_DBG("end sequence");
                        LOG_DBG("Sensors in circuit: %d",sensor_num );
                }
                
        }

// comment block represents the code used to attain a temperature
// senor's ID, not used in general Temperature data program
// leave commented unless attempting to reattain the ID number
/*        int res;

        const struct device *dev = get_ds18b20_device();// 280305f90c0000cc// 

                if(dev == NULL){
                        return 0;
                }

                // loop for gathering the serial number of temperature sensor, NOT FOR DATA COLLECTION OF TEMP
                while(true){
                        res = sensor_sample_fetch(dev);
                        printk("test, res: %d", res);
                        k_sleep(K_MSEC(20000));
                }
                
                



        while(true){
                struct sensor_value temp;
                
                res = sensor_sample_fetch(dev);
                // printk("TEST, dev: %d ", dev->name);
                if(res != 0){
                        printk("sample_fetch() failed: %d\n", res);
                        return res;
                }

                res = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
                if(res != 0){
                        printk("channel_get() failed: %d\n", res);
                        return res;
                }

                printk("Temp: %d.%06d\n", temp.val1, temp.val2);
                k_sleep(K_MSEC(2000));
        }
        */
        return 0;
}

// Function to check if the temperature sensors are present
void w1_search_callback(struct w1_rom rom, void *user_data){
        uint64_t rom_as_int = w1_rom_to_uint64(&rom);
        for(int i = 0; i < KNOWN_SENSORS; i++){
                if(rom_as_int == known_roms[i]){
                        LOG_DBG("Found sensor ID %d with ROM 0x%016llx", known_ids[i], rom_as_int);
                        present[i] = true;
                        return;
                }
        }
        LOG_DBG("Found new sensor with ROM: 0x%016llx", rom_as_int);
}

// function to talk to temperature sensors
void ds18b20_request_temperatures(const struct device *w1){
        int result;
        struct w1_slave_config slave_config;
        slave_config.overdrive = 0;
        result = w1_skip_rom(w1, &slave_config);
        if(result == -ENODEV) {
                LOG_ERR("No such devices found on bus");
                // device not hooked up
                return;
        }
        if(result < 0){
                LOG_ERR("Other error: %d", result);
                return;
        }

        // if no errors, allow to write to temperature sensor
        result = w1_write_byte(w1, DS18B20_CMD_CONVERT_TEMP);

        if(result < 0){
                LOG_ERR("Error writing to device: %d", result);
                return;
        }
        k_msleep(750); // required ~750 ms for writing to take place
}


// after device has been written to, function to read what was found
int ds18b20_get_temp(const struct device *w1, uint64_t rom, float *temp){
        // variable to store the data of the temperature sensor and be manipulated
        uint8_t scratchpad[9];

        // read scratchpad command, found from DS18b20 datasheet
        uint8_t cmd[1] = {0xBE};

        // declaration of variables

        uint16_t raw_temp;
        struct w1_rom rom_struct;
        w1_uint64_to_rom(rom, &rom_struct);
        int result;
        struct w1_slave_config ds18b20_config;
        ds18b20_config.overdrive = 0;
        ds18b20_config.rom = rom_struct;

        // write read to the device to attain the 16 bit data from the sensor
        result = w1_write_read(w1, &ds18b20_config, cmd, 1, scratchpad, sizeof(scratchpad));

        // error reading from write-read
        if(result < 0){
                LOG_ERR("Error reading scratchpad: %d", result);
                return result;
        }

        // compacting first 7 bits of scratchpad to test if what was received makes sense
        result = w1_crc8(scratchpad, sizeof(scratchpad) - 1);

        // if we did not get the intended crc
        // Compares the compacted first 7 bits into the but 8 of scratchpad, 
        //should be the same if it works
        if(result != scratchpad[8]){
                LOG_ERR("CRC error: %d", result);
                return -EIO;
        }

        if(result < 0){
                return result;
        }
        
        if(result < 0){
                return result;
        }
        if(scratchpad[1] >= 8) {
                // The negative temperature stored in twos compliment
                // Scratchpad[1] represents bits 9-16
                // if scratchpad's bits sum to be greater than 7, we know it is negative
                // if this is true, we need to handle appropriately
                raw_temp = (~scratchpad[1]) << 8 | (~scratchpad[0]);
                // ~ represents bitwise not and we must flip all bits of scratchpad (0 --> 1 & 1 --> 0)
                raw_temp = -raw_temp;
                // we have now made it positive so we needed to switch it back to negative

                // The negative case has now been succesfully handled
        }
        raw_temp = (scratchpad[1] << 8) | scratchpad[0];
        // if the negative case was not necessary, we simply need to shift the bits later and or with the early bits
        *temp = 0.0625 * raw_temp; // Final conversion to celsius

        return 0;
}