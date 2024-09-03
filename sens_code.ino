#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "dht11.h"

#define FAN_RELAY_PIN  PB0
#define PUMP_PIN       PB1
#define FAN_THRESHOLD  25
#define DHT11_PIN      PC0

void init_ports() {
    DDRB |= (1 << FAN_RELAY_PIN) | (1 << PUMP_PIN);  // Set fan relay and pump pin as output
    PORTB &= ~((1 << FAN_RELAY_PIN) | (1 << PUMP_PIN));  // Ensure devices are off initially
    
    DDRC &= ~(1 << DHT11_PIN);  // Set DHT11 pin as input
}

void init_timer() {
    // Example: Configure Timer1 for 1-second interrupts
    TCCR1B |= (1 << WGM12);          // CTC mode
    TIMSK1 |= (1 << OCIE1A);         // Enable Timer1 Compare A Match interrupt
    OCR1A = 15624;                   // Set compare match register for 1Hz (assuming 16MHz clock)
    TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
}

void init_interrupts() {
    sei();  // Enable global interrupts
}

void control_devices(int temperature) {
    if (temperature > FAN_THRESHOLD) {
        PORTB |= (1 << FAN_RELAY_PIN);
        PORTB |= (1 << PUMP_PIN);
    } else {
        PORTB &= ~(1 << FAN_RELAY_PIN);
        PORTB &= ~(1 << PUMP_PIN);
    }
}

int read_temperature() {
    int temperature = -1;
    
    // Assuming dht11_read_temperature() returns -1 on failure
    temperature = dht11_read_temperature();
    
    if (temperature == -1) {
        // Handle sensor read error
        PORTB &= ~((1 << FAN_RELAY_PIN) | (1 << PUMP_PIN));  // Turn off devices
    }
    
    return temperature;
}

ISR(TIMER1_COMPA_vect) {
    // Timer interrupt routine
    static int temperature = 0;
    
    temperature = read_temperature();
    control_devices(temperature);
}

int main(void) {
    init_ports();
    init_timer();
    init_interrupts();
    
    while (1) {
        // Main loop can perform other tasks if needed
        // All temperature control is handled by the Timer1 ISR
        // For example, you could have other code here or enter a sleep mode
        _delay_ms(100);  // Small delay to reduce CPU usage
    }
    
    return 0;
}
