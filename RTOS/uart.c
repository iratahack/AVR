#include <stdio.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "stream_buffer.h"

#define TX_ISR  0

#define MAX_UART_STREAM 8
static StreamBufferHandle_t rxStream;
#if TX_ISR
static StreamBufferHandle_t txStream;
#endif

/**
 * @brief Construct the UART RX ISR
 *
 * Read a character from the UART and send it to the UART rxStream.
 */
ISR(USART_RX_vect)
{
    static BaseType_t pxHigherPriorityTaskWoken;
    unsigned char data = UDR0;

    pxHigherPriorityTaskWoken = pdFALSE;
    xStreamBufferSendFromISR(rxStream, &data, 1, &pxHigherPriorityTaskWoken);

    // Exit through the scheduler if a higher priority task was scheduled
    if (pxHigherPriorityTaskWoken)
        portYIELD_FROM_ISR();
}

#if TX_ISR
ISR(USART_UDRE_vect)
{
    static BaseType_t pxHigherPriorityTaskWoken;
    unsigned char data;

    pxHigherPriorityTaskWoken = pdFALSE;
    if (xStreamBufferReceiveFromISR(txStream, &data, 1, &pxHigherPriorityTaskWoken) > 0)
    {
        UDR0 = data;
    }
    else
    {
        // Disable UART TX interrupts
        UCSR0B &= ~_BV(UDRIE0);
    }

    // Exit through the scheduler if a higher priority task was scheduled
    if (pxHigherPriorityTaskWoken)
        portYIELD_FROM_ISR();
}
#endif

static int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream);

#if TX_ISR
    xStreamBufferSend(txStream, &c, 1, portMAX_DELAY);
    // Enable TX interrupt
    UCSR0B |= _BV(UDRIE0);
#else
    while (!(UCSR0A & _BV(UDRE0)))
        ; /* Wait for empty transmit buffer*/
    UDR0 = c;
#endif
    return (c);
}

/**
 * @brief Read a character from the UART
 *
 * @param stream
 * @return Byte read from UART or EOF on error.
 */
static int uart_getchar(FILE *stream)
{
    unsigned char c;

    // Read from the rxStream
    if (xStreamBufferReceive(rxStream, &c, 1, portMAX_DELAY) != 1)
        return (EOF);

    // Convert CR to LF
    if (c == '\r')
        return ('\n');

    return (c);
}

// Setup the output stream for stdio
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void initUART(void)
{
    // Create the TX/RX stream buffers
    // used to communicate to/from the ISR's
    rxStream = xStreamBufferCreate(MAX_UART_STREAM, 1);
#if TX_ISR
    txStream = xStreamBufferCreate(MAX_UART_STREAM, 1);
#endif
    // Setup stdio
    stdout = stderr = &mystdout;
    stdin = &mystdin;

    // Power on the UART
    power_usart0_enable();

    // Set UART baud rate
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    // Configure UART frame format N81
    UCSR0C = 0x06;

    // Enable TX, RX and RX interrupts
    UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
}
