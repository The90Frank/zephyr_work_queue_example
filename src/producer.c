#include "messages.h"

#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
LOG_MODULE_DECLARE(zbus, CONFIG_ZBUS_LOG_LEVEL);

ZBUS_CHAN_DECLARE(custom_data_chan);

void peripheral_thread(void)
{
   struct custom_msg sm = {0};

   while (1)
   {
      sm.msg += 1;

      LOG_INF("Sending custom data {%u}...", sm.msg);

      zbus_chan_pub(&custom_data_chan, &sm, K_MSEC(1000));

      k_msleep(500);
   }
}

K_THREAD_DEFINE(peripheral_thread_id, 1024, peripheral_thread, NULL, NULL, NULL, 5, 0, 0);
