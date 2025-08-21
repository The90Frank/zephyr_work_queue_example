#include "messages.h"
#include "consumer.h"

#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/zbus/zbus.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/__assert.h>

#define LED0_NODE DT_ALIAS(led0)
struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(LED0_NODE, gpios, {0});

void init_led(void)
{
   uint32_t ret = 0;
   if (!device_is_ready(led.port))
   {
      printk("Error: %s device is not ready\n", led.port->name);
      return;
   }

   ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
   if (ret != 0)
   {
      printk("Error %d: failed to configure pin %d (LED '%d')\n",
             ret, led.pin, 0);
      return;
   }
}

LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_CHAN_DEFINE(custom_data_chan,  /* Name */
                 struct custom_msg, /* Message type */

                 NULL, /* Validator */
                 NULL, /* User data */
                 ZBUS_OBSERVERS(
                     thread_handler_sub, fast_handler_lis), /* observers */
                 ZBUS_MSG_INIT(0)                           /* Initial value {0} */
);

/* Thread handler */

ZBUS_SUBSCRIBER_DEFINE(thread_handler_sub, 4);

static void thread_handler_task(void)
{
   const struct zbus_channel *chan;

   while (!zbus_sub_wait(&thread_handler_sub, &chan, K_FOREVER))
   {
      struct custom_msg msg;

      zbus_chan_read(chan, &msg, K_MSEC(200));

      LOG_INF("Custom msg processed by Thread handler: msg = %u",
              msg.msg);

      if (msg.msg % 2 == 0)
      {
         gpio_pin_set(led.port, led.pin, 1);
      }
      else
      {
      }
   }
}

K_THREAD_DEFINE(thread_handler_id, 1024, thread_handler_task, NULL, NULL, NULL, 3, 0, 0);

/* Fast handler */

static void fh_cb(const struct zbus_channel *chan)
{
   const struct custom_msg *msg = zbus_chan_const_msg(chan);

   LOG_INF("Custom msg processed by Fast handler: msg = %u",
           msg->msg);

   if (msg->msg % 2 == 0)
   {
   }
   else
   {
      gpio_pin_set(led.port, led.pin, 0);
   }
}

ZBUS_LISTENER_DEFINE(fast_handler_lis, fh_cb);