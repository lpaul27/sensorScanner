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
                        if(present[i]){
                                int result = ds18b20_get_temp(w1, known_roms[i], &temp);
                                // if there was an error attaining the temperature or talking to sensor
                                if(result < 0){
                                        LOG_ERR("Error reading temperature from sensor %d: %d", known_ids[i], result);
                                        continue;
                                }
                                LOG_INF("Sensor ID %d: %.2f C", known_ids[i], temp);
                        }
                }
                // sleep for 5 seconds before next reading
                k_msleep(5000);
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
        uint64_t rom_as_int = w1_rom_to_uint64t(&rom);
        for(int i = 0; i < KNOWN_SENSORS; i++){
                if(rom_as_int == known_roms[i]){
                        LOG_INF("Found sensor ID %d with ROM 0x%01611x", known_ids[i], rom_as_int);
                        present[i] = true;
                        return;
                }
        }
        LOG_INF("Found new sensor with ROM: 0x%01611x", rom_as_int);
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
int ds18b20_get_temp(const stuct)


