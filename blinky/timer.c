#include <stdio.h>
#include <time.h>
#include <avr/interrupt.h>

// System ticks count
static unsigned int ticks;

//
// Timer ISR, triggers every 10ms
//
ISR(TIMER2_COMPA_vect)
{
    static unsigned char c = 0;
    time_t timer;

    // Toggle the LED every second
    if (c++ == 99)
    {
        c = 0;
        // Toggle LED
        PORTB ^= _BV(PORTB5);
        system_tick();
        time(&timer);
        printf("%s\n", ctime(&timer));
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
    TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);
    // Set bit 5 of DDRB to one - Set digital pin 13 to output mode
//    DDRB |= _BV(DDB5);
    sei();
}
