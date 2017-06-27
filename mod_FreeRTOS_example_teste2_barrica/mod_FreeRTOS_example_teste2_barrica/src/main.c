#include <asf.h>
#include <stdio.h>
#include <conf_demo.h>

void configure_usart(void);
void configure_eeprom(void);

static OLED1_CREATE_INSTANCE(oled1, OLED1_EXT_HEADER);
struct usart_module usart_instance;

void configure_usart(void)
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);

	config_usart.baudrate    = 9600;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;

	while (usart_init(&usart_instance,
	EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	}

	usart_enable(&usart_instance);
}

void configure_eeprom(void)
{	
	enum status_code error_code = eeprom_emulator_init();

//! [check_init_ok]
	//if (error_code == STATUS_ERR_NO_MEMORY) {
	if (error_code == STATUS_ERR_NO_MEMORY) {
		while (true) {
			/* No EEPROM section has been set in the device's fuses */
		}
	}
//! [check_init_ok]
//! [check_re-init]
	else if (error_code != STATUS_OK) {
		/* Erase the emulated EEPROM memory (assume it is unformatted or
		 * irrecoverably corrupt) */
		eeprom_emulator_erase_memory();
		eeprom_emulator_init();
	}
//! [check_re-init]
}

#if (SAMD || SAMR21)
void SYSCTRL_Handler(void)
{
	if (SYSCTRL->INTFLAG.reg & SYSCTRL_INTFLAG_BOD33DET) {
		SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33DET;
		eeprom_emulator_commit_page_buffer();
	}
}
#endif

static void configure_bod(void)
{
#if (SAMD || SAMR21)
	struct bod_config config_bod33;
	bod_get_config_defaults(&config_bod33);
	config_bod33.action = BOD_ACTION_INTERRUPT;
	// BOD33 threshold level is about 3.2V
	config_bod33.level = 48;
	bod_set_config(BOD_BOD33, &config_bod33);
	bod_enable(BOD_BOD33);

	SYSCTRL->INTENSET.reg = SYSCTRL_INTENCLR_BOD33DET;
	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_SYSCTRL);
#endif
}


int main (void)
{
	system_init();
	oled1_init(&oled1);
	configure_eeprom();
	configure_bod();
	configure_usart();
	gfx_mono_init();
	
	//oled1_set_led_state(&oled1, OLED1_LED2_ID, true);
	
	char pressButton1[] = "SAMR21_barUp         ";
	char pressButton2[] = "SAMR21_barDown       ";
	char pressButton3[] = "SAMR21_resetEEPROM...";	// Escreve no máximo 21 caracteres por linha
	
	//char secondLine[]   = "Mina seus cabelo e da";
	//char thirthLine[]   = "hora, seu corpao viol";
	
	uint8_t i;
	
	/*
	for(i = 0; i < sizeof(secondLine) - 1; i++)
		gfx_mono_draw_char(secondLine[i], i*SYSFONT_WIDTH, 1*SYSFONT_HEIGHT, &sysfont);
	
	for(i = 0; i < sizeof(thirthLine) - 1; i++)
		gfx_mono_draw_char(thirthLine[i], i*SYSFONT_WIDTH, 2*SYSFONT_HEIGHT, &sysfont);
		
	for(i = 0; i < sizeof(secondLine) - 1; i++)
		gfx_mono_draw_char(secondLine[i], i*SYSFONT_WIDTH, 3*SYSFONT_HEIGHT, &sysfont);
	*/
	
	//EEPROM
	
	// Valores para a EEPROM (Esses virão da USART)
	
	uint8_t jogador1 = 6;
	uint8_t jogador2 = 2;
	uint8_t auxJogador1;
	uint8_t auxJogador2;
	
	uint8_t page_data[EEPROM_PAGE_SIZE];
	
	//char placar[] = "Placar: ";
	char placar[18];
		
	//    page_data[0] <= placar do jogador1
	//    page_data[1] <= placar do jogador2
	
	page_data[0] = jogador1;
	page_data[1] = jogador2;
	
	eeprom_emulator_write_page(0, page_data);
	page_data[0] = 255;
	page_data[1] = 255;
	
	eeprom_emulator_read_page(0, page_data);
	
	sprintf(placar, "Placar: %.3d x %.3d\n", page_data[0], page_data[1]);
	
	for(i = 0; i < sizeof(placar) - 1; i++)
		gfx_mono_draw_char(placar[i], i*SYSFONT_WIDTH, 0, &sysfont);
	
	do {
		if(oled1_get_button_state(&oled1, OLED1_BUTTON1_ID)){
			usart_write_buffer_wait(&usart_instance, pressButton1, sizeof(pressButton1));
			usart_write_buffer_wait(&usart_instance, '\n', 1);
			for(i = 0; i < sizeof(pressButton1) - 1; i++)
				gfx_mono_draw_char(pressButton1[i], i*SYSFONT_WIDTH, 2*SYSFONT_HEIGHT, &sysfont);
			//delay_cycles_ms(50);															// Não tem necessidade o delay, eu acho
		}
		
		if(oled1_get_button_state(&oled1, OLED1_BUTTON2_ID)){
			usart_write_buffer_wait(&usart_instance, pressButton2, sizeof(pressButton2));
			usart_write_buffer_wait(&usart_instance, '\n', 1);
			for(i = 0; i < sizeof(pressButton2) - 1; i++)
				gfx_mono_draw_char(pressButton2[i], i*SYSFONT_WIDTH, 2*SYSFONT_HEIGHT, &sysfont);
			//delay_cycles_ms(50);															// Não tem necessidade o delay, eu acho
		}
		
		if(oled1_get_button_state(&oled1, OLED1_BUTTON3_ID)){
			usart_write_buffer_wait(&usart_instance, pressButton3, sizeof(pressButton3));
			usart_write_buffer_wait(&usart_instance, '\n', 1);
			for(i = 0; i < sizeof(pressButton3) - 1; i++)
				gfx_mono_draw_char(pressButton3[i], i*SYSFONT_WIDTH, 2*SYSFONT_HEIGHT, &sysfont);
			//delay_cycles_ms(50);															// Não tem necessidade o delay, eu acho
		}
	} while (true);
}