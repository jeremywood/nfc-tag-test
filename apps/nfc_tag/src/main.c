/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <string.h>

#include "sysinit/sysinit.h"
#include "os/os.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"
#include "console/console.h"
#ifdef ARCH_sim
#include "mcu/mcu_sim.h"
#endif

#include <mcu/nrf52_hal.h>
#include <mcu/nrf52_clock.h>
#include <nrf.h>

#include "nrfx_clock.h"
#include "nrfx_nfct.h"

#include "nfc_t4t_lib.h"

#define NDEF_FILE_SIZE 512

static uint8_t ndef_msg_buf[NDEF_FILE_SIZE];

static volatile int g_task1_loops;

static void clock_event_handler(nrfx_clock_evt_type_t event)
{
    switch(event)
    {
        case NRFX_CLOCK_EVT_HFCLK_STARTED:
            /* Activate NFCT only when HFXO is running */
            nrfx_nfct_state_force(NRFX_NFCT_STATE_ACTIVATED);
            break;

        default:
            /* No implementation required */
            break;
    }
}

nrfx_err_t nfc_platform_setup(void)
{
    nrfx_err_t err_code;

    hal_gpio_toggle(LED_1);


    err_code = nrfx_clock_init(clock_event_handler);
    if (err_code == NRFX_ERROR_ALREADY_INITIALIZED)
    {
        err_code = NRFX_SUCCESS;
    }
    else if (err_code != NRFX_SUCCESS)
    {
        return NRFX_ERROR_INTERNAL;
    }

    console_printf("Utils init\n");
    return err_code;
}

static void
nfc_callback(void *context, nfc_t4t_event_t event, const uint8_t *data, size_t dataLength, uint32_t flags)
{
    switch (event)
    {
        case NFC_T4T_EVENT_FIELD_ON:
            hal_gpio_write(LED_1, 1);
            break;
        case NFC_T4T_EVENT_FIELD_OFF:
            hal_gpio_write(LED_1, 0);
            break;
        default:
            break;
    }
}

void nfc_platform_event_handler(nrfx_nfct_evt_t const * p_event)
{
    switch (p_event->evt_id)
    {
        case NRFX_NFCT_EVT_FIELD_DETECTED:
            nrf52_clock_hfxo_request();
            break;

        case NRFX_NFCT_EVT_FIELD_LOST:
            nrf52_clock_hfxo_release();
            break;

        default:
            /* No implementation required */
            break;
    }
}

int
main(int argc, char **argv)
{
    int rc;
    uint32_t err_code;

#ifdef ARCH_sim
    mcu_sim_parse_args(argc, argv);
#endif

    sysinit();

    hal_gpio_init_out(LED_1, 1);
    hal_gpio_init_out(LED_2, 1);
    hal_gpio_init_out(LED_3, 1);
    hal_gpio_init_out(LED_4, 1);

    err_code = nfc_t4t_setup(nfc_callback, NULL);
    console_printf("nfc_t4t_setup() result = %ld\n", err_code);

    err_code = nfc_t4t_ndef_rwpayload_set(ndef_msg_buf, sizeof(ndef_msg_buf));
    console_printf("nfc_t4t_ndef_rwpayload_set() result = %ld\n", err_code);

    err_code = nfc_t4t_emulation_start();
    console_printf("nfc_t4t_emulation_start() result = %ld\n", err_code);

    while (1) {
        ++g_task1_loops;

        /* Wait one second */
        os_time_delay(OS_TICKS_PER_SEC);

        /* Toggle the LED */
        hal_gpio_toggle(LED_4);
    }
    assert(0);

    return rc;
}

