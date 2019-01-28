/*
 * common.h
 *
 *  Created on: 24-07-2015
 *      Author: Arkadiusz Druciak
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

typedef struct {
	uint16_t long_timer; 	// zmienna do odmierzania d³ugich odcinków czasu
} TCFG;

// zmienne globalne dostêpne w innych modu³¹ch
extern const TCFG cfg_pgm PROGMEM;		// dane konfiguracji w pamiêci FLASH
extern TCFG cfg_eem EEMEM;			// dane konfiguracji w pamiêci EEPROM
extern TCFG cfg_ram;				// dane konfiguracji w pamiêci RAM

// definicje okresów do celów testowych - okresy co 20s
#define GODZIN_20s 1
#define GODZIN_40s 2
#define GODZIN_60s 3
#define GODZIN_80s 4
#define GODZIN_100s 5
#define GODZIN_120s 6

// definicje okresów
#define GODZIN_1 180
#define GODZIN_2 360
#define GODZIN_3 540
#define GODZIN_4 720
#define GODZIN_5 900
// co 1080 - daje 6 godzin = (3600 sekund * 6 godzin)/20 sekund = 21600/20 = 1080
#define GODZIN_6 1080

void cfg_check_and_load_defaults( void );  		// sprawdzenie czy pamiêæ EEPROM nie jest pusta; je¿eli jest to wczytanie danych domyœlnych
void cfg_copy_eem_to_ram( void );				// skopiowanie konfiguracji z EEPROM'u do RAM'u
void cfg_copy_ram_to_eem( void );				// skopiowanie konfiguracji z RAM'u do EEPROMU
void cfg_copy_pgm_to_ram( void );				// skopiowanie konfiguracji z FLASH'a do RAM'u
void cfg_load_defaults( void );					// wczytanie domyœlnej konfiguracji

#endif /* COMMON_H_ */
