#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <pin_config.h>
#include <SoftSerial.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <JSONParser.h>
#include <CmdExecutor.h>
#include <config.h>
#include <avr/wdt.h>

static SoftSerial *serial = new SoftSerial(RX, TX);
static char command_buffer[40];
static CmdExecutor *executor = new CmdExecutor();
static JSONParser *parser = new JSONParser();

void setup_watchdog(int i);

void setup()
{
  //cli();

  serial->begin(19200);
  DEBUG_PRINT(F("Setting up peripheral..."));

  pinMode(SET_APPLIANCE_OUTPUT_PIN, OUTPUT);
  pinMode(SET_APPLIANCE_INDICATION_IS_ACTIVE, OUTPUT);
  pinMode(GET_APPLIANCE_ON_OFF_STATUS, INPUT);

  /*
  pinMode(GET_APPLIANCE_TRIGGER, INPUT);

   GIMSK |= (1 << PCIE);   // pin change interrupt enable
   PCMSK |= (1 << PCINT4);  // pin change interrupt enabled for PCINT4

  MCUCR &= ~(_BV(ISC00));// | _BV(ISC00));      //INT0 on low level
  MCUCR |= 1;      //INT0 on low level
  GIMSK = 1 << INT0; //Enable INT0

    sei();                  // enable interrupts
  */

  digitalWrite(SET_APPLIANCE_INDICATION_IS_ACTIVE, 1);
  serial->flush();
  setup_watchdog(9);
  analogWrite(SET_APPLIANCE_OUTPUT_PIN, 0);
  // analogWrite(SET_APPLIANCE_OUTPUT_PIN, 0);
}

void loop()
{
  if (serial->available())
  {
    char ch;
    uint8_t len = 0;
    memset(command_buffer, '\0', sizeof(command_buffer));
    while (serial->available())
    {
      ch = serial->read();
      delay(10);
      command_buffer[len] = ch;
      len++;
    }
    command_buffer[len] = '\0';

    DEBUG_PRINT(F("Received String "));
    DEBUG_PRINT(i);
    DEBUG_PRINT(command_buffer);

    int err = parser->parse(command_buffer);
    if (0 == err)
    {
      for (int i = 0; i < parser->total_pairs; i++)
      {
        DEBUG_PRINT(parser->pairs[i].key);
        DEBUG_PRINT(parser->pairs[i].val);
        executor->execute_command(parser->pairs[i].key, parser->pairs[i].val, serial);
      }

      for (int i = 0; i < parser->total_pairs; i++)
      {
        free(parser->pairs[i].key);
        free(parser->pairs[i].val);
      }
    }
    else
    {
      serial->println("PARSING_ERROR");
    }

    serial->flush();
  }

  analogWrite(SET_APPLIANCE_OUTPUT_PIN, 0);
  delay(300);
  analogWrite(SET_APPLIANCE_OUTPUT_PIN, 255);
  delay(300);

  wdt_reset();
}

void setup_watchdog(int ii)
{
  cli();
  wdt_reset();

  byte bb;
  int ww;
  if (ii > 9)
    ii = 9;
  bb = ii & 7;
  if (ii > 7)
    bb |= (1 << 5);
  bb |= (1 << WDCE);
  ww = bb;

  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  //WDTCR = bb;
  // Set Watchdog settings:
  WDTCR = (1 << WDIE) | (1 << WDE) |
          (1 << WDP3) | (0 << WDP2) | (0 << WDP1) |
          (1 << WDP0);
  // WDTCR |= _BV(WDE);
  sei();
}

/*
ISR(INT0_vect)
{

  int toggle_bulb = digitalRead(2);
  if (toggle_bulb == 1)
    analogWrite(SET_APPLIANCE_OUTPUT_PIN, 255);
  else
    analogWrite(SET_APPLIANCE_OUTPUT_PIN, 0);
}
*/
// Watchdog Interrupt Service / is executed when watchdog timed out
//Keeping for reference
/*ISR(WDT_vect) {
  ...  // set global flag
}*/