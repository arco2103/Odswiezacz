/*
 * main.c
 *
 *  Created on: 2015-03-24
 *       Autor: Arkadiusz Druciak
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include <avr/wdt.h>
#include <avr/sleep.h>

#include "common.h"

/* makrodefinicja do przeprowadzania testów
 * 0 - nie ma testów
 * 1 - test
 * */
#define TEST 0

/* przydatne definicje pinów steruj¹cych */
#define WE_A PC0
#define WE_B PB2
#define H_ENABLE PB1

/* definicje poleceñ steruj¹cych prac¹ silnika */
#define DC_LEWO PORTC &= ~(1<<WE_A); PORTB |= (1<<WE_B)
#define DC_PRAWO PORTC |= (1<<WE_A); PORTB &= ~(1<<WE_B)
#define DC_STOP PORTC &= ~(1<<WE_A); PORTB &= ~(1<<WE_B)
#define DC_ENABLE PORTB &= ~(1<<H_ENABLE);
#define DC_DISABLE PORTB |= (1<<H_ENABLE);

// klawisz u¿ywany do jednorazowego uruchomienia
#define SW1 (1<<PC2)

// funkcja powoduje jednorazowe uruchomienie spryskiwacza
void psiknij(void) {
	DC_ENABLE;
	_delay_ms(1);
	DC_PRAWO;
	_delay_ms(750);
	DC_STOP;
	_delay_ms(150);
	DC_LEWO;
	_delay_ms(750);
	DC_STOP;
	_delay_ms(100);
	DC_DISABLE;
	_delay_ms(1);
}

// funkcja do potwierdzania
void potwierdz(void) {
	DC_ENABLE;
	_delay_ms(1);
	DC_PRAWO;
	_delay_ms(100);
	DC_STOP;
	_delay_ms(100);
	DC_LEWO;
	_delay_ms(100);
	DC_STOP;
	_delay_ms(100);
	DC_DISABLE;
	_delay_ms(1);
}

// zmienne - timery programowe
volatile uint8_t Timer1 = 0; //, Timer2;
uint16_t long_timer_tmp = 0;

// inicjalizacja ustawieñ fabrycznych w pamiêci Flash
TCFG cfg_pgm PROGMEM = {
// ustawienie fabryczne okresu co jaki ma psikn¹æ urz¹dzenie
#if TEST==1
		GODZIN_40s }; // do celów testowych
#else
		GODZIN_6};
#endif

// inicjalizacja zmiennych konfiguracyjnych w pamiêci EEPROM i RAM
TCFG cfg_eem EEMEM; // dane w pamiêci EEPROM
TCFG cfg_ram; // dane w pamiêci RAM

int main(void) {

// blokujemy Watchdog'a bo w przypadku resetu zwi¹zanego z Watchdog'iem jest on ci¹gle aktywny i uk³ad móg³by siê
// w kó³ko resetowaæ
	MCUSR &= ~(1 << WDRF); // skasowanie flagi resetu zwi¹zanego z Watchdog'iem
// zablokowanie Watchdog'a
	wdt_disable();

// ustawiamy piny steruj¹ce mostkiem H jako wyjœcia
	DDRC |= (1 << WE_A);
	DDRB |= (1 << WE_B);
	DDRB |= (1 << H_ENABLE);

// zatrzymujemy silnik
	DC_DISABLE;
	DC_STOP;

// inicjalizacja klawisza
	PORTC |= SW1; // podci¹gamy liniê klawisza do VCC

// konfiguracja timera2 do wybudzania uk³adu z trybu Power Save
// przerwanie bêdzie wywo³ywane raz na sekundê
	ASSR |= (1 << AS2); // tryb asynchroniczny, zewnêtrzny rezonator 32.768kHz
	TCCR2 |= (1 << WGM21); // tryb CTC
	TCCR2 |= (1 << CS22) | (1 << CS21); // preskaler = 256
	OCR2 = 127; // przerwanie co sekundê
	while (ASSR & ((1 << OCR2UB) | (1 << TCR2UB)))
		; // czekamy, a¿ do rejestrów OCR2 i TCCR2 zostan¹ przepisane wartoœci z rejestrów tymczasowych
	TIMSK |= (1 << OCIE2); // zezwolenie na wywo³anie przerwania Compare

// konfiguracja trybu Power Save
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);

// wy³¹czenie niepotrzebnych peryferiów w celu redukcji konsumpcji mocy
	ACSR |= (1 << ACD); //wy³¹czenie komparatora analogowego

// odblokowanie przerwañ
	sei();

// sprawdzenie i wczytanie konfiguracji
	cfg_check_and_load_defaults(); //wczytanie ustawieñ po resecie lub gdy nie ma zapisanych danych w pamiêci EEPROM to wczytanie wartoœci fabrycznych
// inicjalizacja konfiguracji w pamiêci RAM
	cfg_copy_eem_to_ram();

// sprawdzenie czy jest wciœniêty klawisz podczas resetu,
// je¿eli tak to wchodzimy do ustawieñ odœwierzacza,
	uint8_t i = 0; // zmienna pomocnicza do wyboru d³ugoœci przerwy miêdzy psikniêciami

	if (!(PINC & SW1)) {
		// eliminacja drgañ styków
		_delay_ms(10);
		if (!(PINC & SW1)) {

			Timer1 = 7; // ustawiamy Timer 1 na 5 sekund

			// czekamy na zwolnienie przycisku
			while (1) {
				if ((PINC & SW1)) {
					_delay_ms(10); // eliminacja drgañ styków
					if ((PINC & SW1)) {
						break;
					}
				}
				// je¿eli przycisk wciœniêty d³u¿ej ni¿ 7 sekund to przywracamy ustawienia fabryczne
				// i wracamy do pêtli g³ównej
				if (!Timer1) {
					cfg_load_defaults();
					potwierdz();
					potwierdz();
					_delay_ms(3000);
					goto petla_gl;
				}
			}


			// weszliœmy do menu
			potwierdz();
			potwierdz();

			Timer1 = 5; // ustawiamy Timer 1 na 5 sekund
			// pêtla zlicza iloœæ wciœniêæ przez u¿ytkownika przycisku
			// ka¿de przyciœniêcie oznacza zmianê okresu wg. zasady
			// pierwsze - to okres 1 godzinny,
			// drugie  -  to okres 2 godzinny i tak dalej a¿ do 6 godzin
			// je¿eli przerwiemy przyciskanie d³u¿ej ni¿ 5 sekund to zostanie zapamiêtany okres i wrócimy
			// do pêtli g³ównej
			while (1) {
				if (!Timer1)
					break;
				// sprawdzamy czy przycisk wciœniêty
				if (!(PINC & SW1)) {
					// eliminacja drgañ styków
					_delay_ms(10);
					if (!(PINC & SW1)) {

						// czekamy na zwolnienie przycisku
						while (1) {
							if ((PINC & SW1)) {
								_delay_ms(10); // eliminacja drgañ styków
								if ((PINC & SW1)) {
									break;
								}
							}
						}

						if (i == 6)
							i = 0; // zaczynamy ponownie odliczanie

						// zwiêkszamy licznik okresów
						i++;
						for (uint8_t x = 0; x < i; x++) {
							potwierdz(); // potwierdzamy wybór
							_delay_ms(50);
						} //end for
						Timer1 = 5; // ponownie zaczynamy odliczanie 5 sekund
					} // end if
				} //end if
			} //end while

			// w zale¿noœci od zmiennej i wybieramy okres przerwy miêdzy psikniêciami
			switch (i) {
			case 0:
				// idziemy do pêtli g³ównej bo u¿ytkownk nie wcin¹³ przyciku
				break;
			case 1:
#if TEST==1
				cfg_ram.long_timer = GODZIN_20s; // do celów testowych
#else
				cfg_ram.long_timer = GODZIN_1;
#endif
				break;
			case 2:
#if TEST==1
				cfg_ram.long_timer = GODZIN_40s; // do celów testowych
#else
				cfg_ram.long_timer = GODZIN_2;
#endif
				break;
			case 3:
#if TEST==1
				cfg_ram.long_timer = GODZIN_60s; // do celów testowych
#else
				cfg_ram.long_timer = GODZIN_3;
#endif
				break;
			case 4:
#if TEST==1
				cfg_ram.long_timer = GODZIN_80s; // do celów testowych
#else
				cfg_ram.long_timer = GODZIN_4;
#endif
				break;
			case 5:
#if TEST==1
				cfg_ram.long_timer = GODZIN_100s; // do celów testowych
#else
				cfg_ram.long_timer = GODZIN_5;
#endif
				break;
			case 6:
#if TEST==1
				cfg_ram.long_timer = GODZIN_120s; // do celów testowych
#else
				cfg_ram.long_timer = GODZIN_6;
#endif
				break;
			}
			cfg_copy_ram_to_eem();
			Timer1 = 0;
		}
	}

	petla_gl:
// odblokowanie watchdoga
// timer na 2 sekundy
	wdt_enable(WDTO_2S);

	potwierdz();
	while (1) {

		// sprawdzenie czy jest wciœniêty klawisz (trzeba przytrzymaæ do 1 sekundy)
		if (!(PINC & SW1)) {
			// eliminacja drgañ styków
			_delay_ms(10);
			if (!(PINC & SW1)) {
				long_timer_tmp = 0;
				psiknij();
			}
		}

		if (!Timer1) {
			Timer1 = 20; // co 20 sekund
			long_timer_tmp++;

			if (long_timer_tmp == cfg_ram.long_timer) {
				long_timer_tmp = 0;
				psiknij();
			}
		}

		// Prze³¹czenie uk³adu w tryb Power Save
		// Poniewa¿ u¿ywamy Timera2 do wybudzenia
		// to nale¿y spe³niæ warunek, aby od czasu wybudzenia
		// do czasu ponownego uœpienia na TOSC1 wyst¹pi³ chocia¿ jeden takt.
		// Niezbêdne to jest do zresetowania logiki przerwañ.
		_delay_us(100);
		set_sleep_mode(SLEEP_MODE_PWR_SAVE);
		// wybranie trybu Power Save
		sleep_mode(); // dobranoc
	}
}

// obs³uga timerów programowych
ISR( TIMER2_COMP_vect) {
	uint8_t n;

// zresetowanie watchdoga
	wdt_reset();
	n = Timer1; /* 1Hz Timer1 */
	if (n)
		Timer1 = --n;
}

