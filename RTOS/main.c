#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void initUART(void);

#define mainLED_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
  sleep_cpu();
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  cli();
  printf("PANIC! Stack overflow (%s)\n", pcTaskName);
  for (;;)
    ;
}

void vLEDFlashTask(void *pvParms)
{
  TickType_t xLastWakeTime;
  printf("Hello from %s\n", __func__);

  // Enable LED port direction to output
  DDRB |= _BV(PB5);

  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    // Wait for the next cycle.
    xTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ);

    // Perform action here.
    PORTB ^= _BV(PB5);
    printf("%02x\n", PORTB);
  }
}

int main(void)
{
  /* On AVR devices all peripherals are enabled from power on reset, this
   * disables all peripherals to save power. Driver shall enable
   * peripheral if used */
  PRR = 0xff;

  initUART();

  printf("Hello from %s\n", __func__);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  xTaskCreate(vLEDFlashTask, "LED", configMINIMAL_STACK_SIZE, NULL, mainLED_TASK_PRIORITY, NULL);
  vTaskStartScheduler();

  return 0;
}
