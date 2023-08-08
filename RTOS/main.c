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
  printf("Hello from %s\n", __func__);

  DDRB |= _BV(PB5);

  for (;;)
  {
    PORTB ^= _BV(PB5);
    printf("%02x\n", PORTB);
    vTaskDelay(66);
  }
}

int main(void)
{
  initUART();

  printf("Hello from %s\n", __func__);

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();

  xTaskCreate(vLEDFlashTask, "LED", configMINIMAL_STACK_SIZE, NULL, mainLED_TASK_PRIORITY, NULL);
  vTaskStartScheduler();

  return 0;
}
