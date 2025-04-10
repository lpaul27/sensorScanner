#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sensor/w1_sensor.h>
#include <zephyr/logging/log.h>

// struct device *tempSensors[1];
// uint64_t sensorIDs [1] = {0x280305f90c0000cc};

void w1_search_callback(struct w1_rom rom, void *user_data);
void ds18b20_request_temperatures(const struct device *w1);
int ds18b20_get_temp(const struct device *w1, uint64_t rom, float *temp);

#define DS18B20_CMD_CONVERT_TEMP 0x44
#define KNOWN_SENSORS 2 


uint64_t known_roms[KNOWN_SENSORS] = {
        // list of all known sensors in order
        0x280305f90c0000cc, //1 
        0x28d676f90c0000c0, //2 
};

uint32_t known_ids[KNOWN_SENSORS] = {
        // List of the number of known sensors 
        // in accordance to the ID's above
        1,
        2,
};
// boolean to represent whether or not the sensor is working
bool present[KNOWN_SENSORS] = {false, false};

// Array to store the temperature data
float temp_readings[KNOWN_SENSORS];

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
        float temp;

        if(!device_is_ready(w1)) {
                LOG_ERR("W1 device not ready");
                return -1;
        }

        LOG_INF("Searching for 1-wire sensors");
        int num_sensors = w1_search_rom(w1, w1_search_callback, NULL);

        // infinite loop for attaining temperature data
        while(true){
                ds18b20_request_temperatures(w1);
                for(int i = 0; i < KNOWN_SENSORS; i++){
                        temp = -666;
                        if(present[i]){
                                int result = ds18b20_get_temp(w1, known_roms[i], &temp);
                                // if there was an error attaining the temperature or talking to sensor
                                if(result < 0){
                                        LOG_ERR("Error reading temperature from sensor %d: %d", known_ids[i], result);
                                }
                                // LOG_INF("Sensor ID %d: %.2f C", known_ids[i], temp);
                        }
                        printk("%.2f\t", temp);
                }
                // sleep for 5 seconds before next reading

                //print data from the for loop in one line string for the python code to read
                printk("\n");
                
                k_msleep(2000);
        }

// comment block represents the code used to attain a temperature
// senor's ID, not used in general Temperatur attaining
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
                        LOG_INF("Found sensor ID %d with ROM 0x%016llx", known_ids[i], rom_as_int);
                        present[i] = true;
                        return;
                }
        }
        LOG_INF("Found new sensor with ROM: 0x%016llx", rom_as_int);
}

// function to talk to temperature sensors
void ds18b20_request_temperatures(const struct device *w1){
        int result;
        struct w1_slave_config slave_config;
        slave_config.overdrive = 0;
        result = w1_skip_rom(w1, &slave_config);
        if(result == -ENODEV) {
                LOG_ERR("No such devices foudn on bus");
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
        k_msleep(751); // required 750 ms for writing to take place
}


// after device has been written to, function to read what was found
int ds18b20_get_temp(const struct device *w1, uint64_t rom, float *temp){
        // variable to store the data of the temperature sensor and be manipulated
        uint8_t scratchpad[9];

        // read scratchpad command
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
        // Compares the compacted first 7 bits into the but 8 of scratchpad, should be the same if it works
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


