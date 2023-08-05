#include <time.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

// System ticks count
static unsigned int ticks;

//
// Timer ISR
//
ISR(TIMER2_COMPA_vect)
{
    static unsigned char c = 0;

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
void setupTimer(void)
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

int main(void)
{
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    sleep_enable();

    // Set bit 5 of DDRB to one - Set digital pin 13 to output mode
    DDRB |= _BV(DDB5);

    setupTimer();

    // Enable interrupts
    sei();

    while (1)
    {
        sleep_cpu();
    }
}
