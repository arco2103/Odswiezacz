/*
 * common.c
 *
 *  Created on: 24-07-2015
 *      Author: Arkadiusz Druciak
 */


#include <avr/io.h>
#include "common.h"

// ****************************************************************************
//
// cfg_copy_eem_to_ram 		funkcja do kopiowania konfiguracji z pami�ci
//							EEPROM do RAM
//
// ****************************************************************************
// wej�cie: - brak
// wyj�cie: - brak
void cfg_copy_eem_to_ram( void ) {
	eeprom_read_block( &cfg_ram, &cfg_eem, sizeof(cfg_ram) );
}

// ****************************************************************************
//
// cfg_copy_ram_to_eem 		funkcja do kopiowania konfiguracji z pami�ci
//							RAM do EEPROM
//
// ****************************************************************************
// wej�cie: - brak
// wyj�cie: - brak
void cfg_copy_ram_to_eem( void ) {
	eeprom_write_block( &cfg_ram, &cfg_eem, sizeof(cfg_ram) );
}

// ****************************************************************************
//
// cfg_copy_pgm_to_ram 		funkcja do kopiowania konfiguracji z pami�ci
//							FLASH do RAM
//
// ****************************************************************************
// wej�cie: - brak
// wyj�cie: - brak
void cfg_copy_pgm_to_ram( void ) {
	memcpy_P( &cfg_ram, &cfg_pgm, sizeof(cfg_ram) );
}

// ****************************************************************************
//
// cfg_load_defaults 		funkcja wczytuje fabryczne parametry konfiguracji
//							z pami�ci FLASH do RAM i do EEPROM
//
// ****************************************************************************
// wej�cie: - brak
// wyj�cie: - brak
void cfg_load_defaults( void ) {
	cfg_copy_pgm_to_ram();
	cfg_copy_ram_to_eem();
}

// ****************************************************************************
//
// cfg_check_and_load_defaults 		funkcja sprawdza czy s� dane w EEPROM
//									je�eli nie ma to wczytuje warto�ci domy�lne
//									z pami�ci FLASH do pami�ci RAM i EEPROM
//
// ****************************************************************************
// wej�cie: - brak
// wyj�cie: - brak
void cfg_check_and_load_defaults( void ) {
	uint8_t i, len = sizeof( cfg_ram );
	uint8_t * ram_wsk = (uint8_t*)&cfg_ram;

	cfg_copy_eem_to_ram();
	for(i=0; i<len; i++) {
		if( 0xff == *ram_wsk++ ) continue;
		break;
	}

	if( i == len ) {
		cfg_load_defaults();
	}

}

