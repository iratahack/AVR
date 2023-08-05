#include <stdio.h>
#include <time.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

// System ticks count
static unsigned int ticks;

// Setup the output stream for stdio
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
static FILE mystdin = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

static int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        fputc('\r', stream);

    while (!(UCSR0A & _BV(UDRE0)))
        ; /* Wait for empty transmit buffer*/
    UDR0 = c;

    return (c);
}

static int uart_getchar(FILE *stream)
{
    char c;
    while (!(UCSR0A & _BV(RXC0)))
        ;
    c = UDR0;

    if (c == '\r')
        c = '\n';

    // Echo characters
    uart_putchar(c, stdout);

    return (c);
}

//
// Timer ISR, triggers every 10ms
//
ISR(TIMER2_COMPA_vect)
{
    static unsigned char c = 0;

    // Toggle the LED every second
    if (c++ == 100)
    {
        c = 0;
        // Toggle LED
        PORTB ^= _BV(PORTB5);
        system_tick();
    }
    ticks++;
}

//
// Timer tick 100/s
//
void initTimer(void)
{
    // Enable timer COMPA interrupt
    TIMSK2 |= _BV(OCIE2A);
    // Set timer compare value
    OCR2A = (F_CPU / 1024 / 100);
    // Timer mode (2) CTC
    TCCR2A = _BV(WGM21);
    // Select timer clock
    TCCR2B = 7; // clk/1024
}

void initUART(void)
{
    stdout = stderr = &mystdout;
    stdin = &mystdin;
#undef BAUD // avoid compiler warning
#define BAUD 1000000
#include <util/setbaud.h>
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
    UCSR0C = 0x06; /* Set frame format: 8data, 1stop bit  */
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
}

unsigned char data[] = "Hello from ATmega328p\n";

int main(void)
{
    char s[16];
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();

    // Set bit 5 of DDRB to one - Set digital pin 13 to output mode
    DDRB |= _BV(DDB5);

    initUART();
    initTimer();

    // Enable interrupts
    sei();

    gets(s);

    printf("%s\n", s);

    printf(data);

    printf("UBRR0H = %d, UBRR0L = %d\n", UBRR0H, UBRR0L);

    while (1)
    {
        sleep_cpu();
    }
}
