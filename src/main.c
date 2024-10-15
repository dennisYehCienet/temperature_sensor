#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

#define DHT22_NODE DT_INST(0, aosong_dht)

static const char *now_str(void)
{
	static char buf[16]; /* ...HH:MM:SS.MMM */
	uint32_t now = k_uptime_get_32();
	unsigned int ms = now % MSEC_PER_SEC;
	unsigned int s;
	unsigned int min;
	unsigned int h;

	now /= MSEC_PER_SEC;
	s = now % 60U;
	now /= 60U;
	min = now % 60U;
	now /= 60U;
	h = now;

	snprintk(buf, sizeof(buf), "%u:%02u:%02u.%03u",
		 h, min, s, ms);
	return buf;
}

void main(void)
{
	// const char *const label = DT_LABEL(DT_INST(0, aosong_dht));
	// const struct device *dht22 = device_get_binding("DHT22");

        const struct device *dht22 = device_get_binding(DT_PROP(DHT22_NODE, label));
        if (!dht22) {
                printk("Could not get binding for DHT device\n");
                return;
        }

	// if (!dht22) {
	// 	printk("Failed to find sensor %s\n", label);
	// 	return;
	// }

	while (true) {
		int rc = sensor_sample_fetch(dht22);

		if (rc != 0) {
			printk("Sensor fetch failed: %d\n", rc);
			continue;
		}

		struct sensor_value temperature;
		struct sensor_value humidity;

		rc = sensor_channel_get(dht22, SENSOR_CHAN_AMBIENT_TEMP,
					&temperature);
		if (rc == 0) {
			rc = sensor_channel_get(dht22, SENSOR_CHAN_HUMIDITY,
						&humidity);
		}
		if (rc != 0) {
			printk("get failed: %d\n", rc);
			continue;
		}
		// printk("temperature.value1 = %d, temperature.value2 = %d\n",temperature.val1, temperature.val2);

		printk("[%s]: %.1f Cel ; %.1f %%RH\n",
		       now_str(),
		       sensor_value_to_double(&temperature),
		       sensor_value_to_double(&humidity));
		k_sleep(K_SECONDS(2));
	}
}
