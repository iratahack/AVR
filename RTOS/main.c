#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/eeprom.h>

void initUART(void);

EEMEM char test[] = "Hello";

#define mainLED_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define input_TASK_PRIORITY (mainLED_TASK_PRIORITY)

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

void inputTask(void *pvParms)
{
  char s;
  printf("%s running.\n", __func__);
  for (;;)
  {
    fread(&s, 1, 1, stdin);
    fwrite(&s, 1, 1, stdout);
  }
}

void vLEDFlashTask(void *pvParms)
{
  TickType_t xLastWakeTime;
  printf("%s running.\n", __func__);

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
  }
}

int main(void)
{
  power_all_disable();

  initUART();

  printf("%s running.\n", __func__);

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();

#if 0
  for (int n = 0; n < 1024; n++)
  {
    printf("0x%02x ", eeprom_read_byte((void *)n));
    if ((n & 0x0f) == 0xf)
      printf("\n");
  }
#endif

  xTaskCreate(vLEDFlashTask, "LED", configMINIMAL_STACK_SIZE, NULL, mainLED_TASK_PRIORITY, NULL);
  xTaskCreate(inputTask, "Input", 128, NULL, input_TASK_PRIORITY, NULL);
  vTaskStartScheduler();

  return 0;
}
