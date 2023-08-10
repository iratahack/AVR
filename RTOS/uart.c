#include <stdio.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "stream_buffer.h"

#define MAX_RX_STREAM 8
StreamBufferHandle_t rxStream;
BaseType_t pxHigherPriorityTaskWoken;

/**
 * @brief Construct the UART RX ISR
 *
 * Read a character from the UART and send it to the UART rxStream.
 */
ISR(USART_RX_vect)
{
    unsigned char data = UDR0;

    pxHigherPriorityTaskWoken = pdFALSE;
    xStreamBufferSendFromISR(rxStream, &data, 1, &pxHigherPriorityTaskWoken);

    // Exit through the scheduler if a higher priority task was scheduled
    if (pxHigherPriorityTaskWoken)
        portYIELD_FROM_ISR();
}

static int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream);

    while (!(UCSR0A & _BV(UDRE0)))
        ; /* Wait for empty transmit buffer*/
    UDR0 = c;

    return (c);
}

static int uart_getchar(FILE *stream)
{
    unsigned char c;

    // Read from the rxStream
    xStreamBufferReceive(rxStream, &c, 1, portMAX_DELAY);

    // Convert CR to LF
    if (c == '\r')
        c = '\n';

    return (c);
}

// Setup the output stream for stdio
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void initUART(void)
{
    rxStream = xStreamBufferCreate(MAX_RX_STREAM, 1);
    // Power on the UART
    power_usart0_enable();
    // Setup stdio
    stdout = stderr = &mystdout;
    stdin = &mystdin;
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
    UCSR0C = 0x06; /* Set frame format: 8data, 1stop bit  */
    // Enable TX, RX and RX interrupts
    UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
}
