/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-27     Ernest Chen  the first version
 */

#include <rtthread.h>

#include "stm32l4xx_hal.h"

static struct rt_event led_event;

static struct rt_thread led_thread1;
ALIGN(RT_ALIGN_SIZE)
static char led_thread1_stack[512];

static struct rt_thread led_thread2;
ALIGN(RT_ALIGN_SIZE)
static char led_thread2_stack[512];

static struct rt_thread led_thread3;
ALIGN(RT_ALIGN_SIZE)
static char led_thread3_stack[512];

static struct rt_thread key_thread;
ALIGN(RT_ALIGN_SIZE)
static char key_thread_stack[512];

static rt_uint32_t reversal_times = 1000; // ms

static void led_thread1_entry(void *parameter)
{
    rt_uint32_t e;
    int i;

    while (1)
    {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET); //red
        if (rt_event_recv(&led_event, (1 << 7), RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &e) == RT_EOK)
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
        }
        rt_thread_mdelay(reversal_times);
    }
}

static void led_thread2_entry(void *parameter)
{
    rt_uint32_t e;
    int i;

    while (1)
    {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET); //green
        if (rt_event_recv(&led_event, (1 << 8), RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &e) == RT_EOK)
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
        }
        rt_thread_mdelay(reversal_times);
    }
}

static void led_thread3_entry(void *parameter)
{
    rt_uint32_t e;
    int i;

    while (1)
    {
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET); //blue
        if (rt_event_recv(&led_event, (1 << 9), RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &e) == RT_EOK)
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
        }
        rt_thread_mdelay(reversal_times);
    }
}

static void key_thread_entry(void *parameter)
{
    while (1)
    {
        if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == RESET)
        {
            rt_event_send(&led_event, (1 << 7)); //red
        }
        else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9) == RESET)
        {
            rt_event_send(&led_event, (1 << 8)); //green
        }
        else if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10) == RESET)
        {
            rt_event_send(&led_event, (1 << 9)); //blue
        }
        rt_thread_mdelay(reversal_times);
    }
}

void MX_rtthread_Process(void)
{
    /* close all led */
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_7, GPIO_PIN_SET);

    /* init event */
    rt_event_init(&led_event, "led_event", RT_IPC_FLAG_FIFO);

    /* init led1 device thread */
    rt_thread_init(&led_thread1,
                   "led1",
                   led_thread1_entry, RT_NULL,
                   led_thread1_stack, sizeof(led_thread1_stack),
                   3, 20);
    rt_thread_startup(&led_thread1);

    /* init led2 device thread */
    rt_thread_init(&led_thread2,
                   "led2",
                   led_thread2_entry, RT_NULL,
                   led_thread2_stack, sizeof(led_thread2_stack),
                   3, 20);
    rt_thread_startup(&led_thread2);

    /* init led3 device thread */
    rt_thread_init(&led_thread3,
                   "led3",
                   led_thread3_entry, RT_NULL,
                   led_thread3_stack, sizeof(led_thread3_stack),
                   3, 20);
    rt_thread_startup(&led_thread3);

    /* init key device thread */
    rt_thread_init(&key_thread,
                   "key",
                   key_thread_entry, RT_NULL,
                   key_thread_stack, sizeof(key_thread_stack),
                   5, 20);
    rt_thread_startup(&key_thread);
}
