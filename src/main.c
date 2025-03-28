#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>


static const struct device *get_ds18b20_device(void){
        const struct device *const dev = DEVICE_DT_GET_ANY(maxim_ds18b20);

        if(dev == NULL){
                // no node found or does not have status 'okay'
                printk("\nError: no device found.\n");
                return NULL;
        }

        if(!device_is_ready(dev)){
                printk("\nError: Device \"%s\" is not ready; "
                        "check the driver initialization logs forn errors. \n",
                        dev->name);
                return NULL;
        }

        printk("Found device \"%s\", getting sensor data\n", dev->name);
        return dev;
} // end get function


int main(void){

        int res;

        const struct device *dev = get_ds18b20_device();

                if(dev == NULL){
                        return 0;
                }


        while(true){
                res = sensor_sample_fetch(dev);
                k_sleep(K_MSEC(2000));
        }
        // while(true){
        //         struct sensor_value temp;

        //         res = sensor_sample_fetch(dev);
        //         if(res != 0){
        //                 printk("sample_fetch() failed: %d\n", res);
        //                 return res;
        //         }

        //         res = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        //         if(res != 0){
        //                 printk("channel_get() failed: %d\n", res);
        //                 return res;
        //         }

        //         printk("Temp: %d.%06d\n", temp.val1, temp.val2);
        //         k_sleep(K_MSEC(2000));
        // }
        return 0;
}
