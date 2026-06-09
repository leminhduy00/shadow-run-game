#include "Arduino.h"
#include "io_cfg.h"
#include "sys_dbg.h"

void pinMode(uint8_t pin, uint8_t mode) {
	switch (pin) {
	case OLED_SPI_SCK_PIN:
		if (mode == INPUT) {
			oled_sck_input_mode();
		}
		else if (mode == OUTPUT) {
			oled_sck_output_mode();
		}
		else if (mode == INPUT_PULLUP) {
			oled_sck_input_mode();
		}
		else {
			FATAL("AR", 0x01);
		}
		break;

	case OLED_SPI_MOSI_PIN:
		if (mode == INPUT) {
			oled_mosi_input_mode();
		}
		else if (mode == OUTPUT) {
			oled_mosi_output_mode();
		}
		else if (mode == INPUT_PULLUP) {
			oled_mosi_input_mode();
		}
		else {
			FATAL("AR", 0x01);
		}
		break;

	case OLED_CS_PIN:
		if (mode == INPUT) {
			oled_cs_input_mode();
		}
		else if (mode == OUTPUT) {
			oled_cs_output_mode();
		}
		else if (mode == INPUT_PULLUP) {
			oled_cs_input_mode();
		}
		else {
			FATAL("AR", 0x01);
		}
		break;

	case OLED_DC_PIN:
		if (mode == INPUT) {
			oled_dc_input_mode();
		}
		else if (mode == OUTPUT) {
			oled_dc_output_mode();
		}
		else if (mode == INPUT_PULLUP) {
			oled_dc_input_mode();
		}
		else {
			FATAL("AR", 0x01);
		}
		break;

	case OLED_RES_PIN:
		if (mode == INPUT) {
			oled_rst_input_mode();
		}
		else if (mode == OUTPUT) {
			oled_rst_output_mode();
		}
		else if (mode == INPUT_PULLUP) {
			oled_rst_input_mode();
		}
		else {
			FATAL("AR", 0x01);
		}
		break;

	default:
		FATAL("AR", 0xF1);
		break;
	}
}

void digitalWrite(uint8_t pin, uint8_t val) {
	switch (pin) {
	case OLED_SPI_SCK_PIN:
		if (val == HIGH) {
			oled_sck_digital_write_high();
		}
		else if (val == LOW) {
			oled_sck_digital_write_low();
		}
		else {
			FATAL("AR", 0x02);
		}
		break;

	case OLED_SPI_MOSI_PIN:
		if (val == HIGH) {
			oled_mosi_digital_write_high();
		}
		else if (val == LOW) {
			oled_mosi_digital_write_low();
		}
		else {
			FATAL("AR", 0x02);
		}
		break;

	case OLED_CS_PIN:
		if (val == HIGH) {
			oled_cs_digital_write_high();
		}
		else if (val == LOW) {
			oled_cs_digital_write_low();
		}
		else {
			FATAL("AR", 0x02);
		}
		break;

	case OLED_DC_PIN:
		if (val == HIGH) {
			oled_dc_digital_write_high();
		}
		else if (val == LOW) {
			oled_dc_digital_write_low();
		}
		else {
			FATAL("AR", 0x02);
		}
		break;

	case OLED_RES_PIN:
		if (val == HIGH) {
			oled_rst_digital_write_high();
		}
		else if (val == LOW) {
			oled_rst_digital_write_low();
		}
		else {
			FATAL("AR", 0x02);
		}
		break;

	default:
		FATAL("AR", 0xF2);
		break;
	}
}

int digitalRead(uint8_t pin) {
	int val = 0;
	switch (pin) {
	case OLED_SPI_SCK_PIN: {
		val = oled_sck_digital_read();
	}
	break;

	case OLED_SPI_MOSI_PIN: {
		val = oled_mosi_digital_read();
	}
	break;

	case OLED_CS_PIN: {
		val = oled_cs_digital_read();
	}
	break;

	case OLED_DC_PIN: {
		val = oled_dc_digital_read();
	}
	break;

	case OLED_RES_PIN: {
		val = oled_rst_digital_read();
	}
	break;

	default:
		FATAL("AR", 0xF3);
		break;
	}
	return val;
}
