#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>

struct device *tempSensors[1];
uint64_t sensorIDs [1] = {0x280305f90c0000cc};

static const struct device *get_ds18b20_device(void){
        const struct device *const dev = DEVICE_DT_GET_ANY(maxim_ds18b20);

        if(dev == NULL){
                // no node found or does not have status 'okay'
                printk("\nError: no device found.\n");
                return NULL;
        }

        if(!device_is_ready(dev)){
                printk("\nError: Device \"%s\" is not ready; "
                        "check the driver initialization logs for errors. \n",
                        dev->name);
                return NULL;
        }

        printk("Found device \"%s\", getting sensor data\n", dev->name);
        printk("test, \"%s\"", dev ->name);

        if (dev->name == "280305f90c0000cc") {
                printk("Found 1\n");
                tempSensors[0] = &dev;
        }
        return dev;
} // end get function


int main(void){

        int res;

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
        return 0;
}
