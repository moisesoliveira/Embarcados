#include <asf.h>
#include <stdio.h>
#include <conf_demo.h>
#include <string.h>
#define MAX_RX_BUFFER_LENGTH   9
#define MAX_PLACAR             18

volatile uint8_t rx_buffer[MAX_RX_BUFFER_LENGTH];

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
	
	uint8_t i;	
	uint8_t page_data[EEPROM_PAGE_SIZE];
	uint8_t placar[] = "Placar: ";
	uint8_t ox[]     = " x ";

	do {
		
		for(i = 0; i < sizeof(rx_buffer) - 1; i++)
			rx_buffer[i] = 0;
			
		//usart_read_buffer_job(&usart_instance, (uint8_t *)rx_buffer, MAX_RX_BUFFER_LENGTH);
		usart_read_buffer_wait (&usart_instance, rx_buffer, MAX_RX_BUFFER_LENGTH);
		if(rx_buffer[0] == 'p'){
			page_data[0] = rx_buffer[1];
			page_data[1] = rx_buffer[2];
			page_data[2] = rx_buffer[3];
			
			page_data[3] = rx_buffer[5];
			page_data[4] = rx_buffer[6];
			page_data[5] = rx_buffer[7];
				
			eeprom_emulator_write_page(0, page_data);
			eeprom_emulator_commit_page_buffer();
			
			page_data[0] = 255;
			page_data[1] = 255;
			page_data[2] = 255;
			page_data[3] = 255;
			page_data[4] = 255;
			page_data[5] = 255;
				
			eeprom_emulator_read_page(0, page_data);
			
			for(i = 0; i < sizeof(rx_buffer) - 1; i++)
				gfx_mono_draw_char(' ', i*SYSFONT_WIDTH, 3*SYSFONT_HEIGHT, &sysfont);
			
			// Desenho do placar
			for(i = 0; i < sizeof(placar) - 1; i++)
				gfx_mono_draw_char(placar[i], i*SYSFONT_WIDTH, 0, &sysfont);
				
			gfx_mono_draw_char(page_data[0], (i++)*SYSFONT_WIDTH, 0, &sysfont);
			gfx_mono_draw_char(page_data[1], (i++)*SYSFONT_WIDTH, 0, &sysfont);
			gfx_mono_draw_char(page_data[2], (i++)*SYSFONT_WIDTH, 0, &sysfont);
			
			gfx_mono_draw_char(ox[0], (i++)*SYSFONT_WIDTH, 0, &sysfont);
			gfx_mono_draw_char(ox[1], (i++)*SYSFONT_WIDTH, 0, &sysfont);
			gfx_mono_draw_char(ox[2], (i++)*SYSFONT_WIDTH, 0, &sysfont);
			
			gfx_mono_draw_char(page_data[3], (i++)*SYSFONT_WIDTH, 0, &sysfont);
			gfx_mono_draw_char(page_data[4], (i++)*SYSFONT_WIDTH, 0, &sysfont);
			gfx_mono_draw_char(page_data[5], i*SYSFONT_WIDTH, 0, &sysfont);
				
			// Fim do desenho do placar
				
		}
		
		if(oled1_get_button_state(&oled1, OLED1_BUTTON1_ID)){
			usart_write_buffer_wait(&usart_instance, pressButton1, sizeof(pressButton1));
			usart_write_buffer_wait(&usart_instance, '\n', 1);
			for(i = 0; i < sizeof(pressButton1) - 1; i++)
				gfx_mono_draw_char(pressButton1[i], i*SYSFONT_WIDTH, 2*SYSFONT_HEIGHT, &sysfont);
			//delay_cycles_ms(50);															// Não tem necessidade o delay, eu acho
		} else if(oled1_get_button_state(&oled1, OLED1_BUTTON2_ID)){
			usart_write_buffer_wait(&usart_instance, pressButton2, sizeof(pressButton2));
			usart_write_buffer_wait(&usart_instance, '\n', 1);
			for(i = 0; i < sizeof(pressButton2) - 1; i++)
				gfx_mono_draw_char(pressButton2[i], i*SYSFONT_WIDTH, 2*SYSFONT_HEIGHT, &sysfont);
			//delay_cycles_ms(50);															// Não tem necessidade o delay, eu acho
		} else if(oled1_get_button_state(&oled1, OLED1_BUTTON3_ID)){
			usart_write_buffer_wait(&usart_instance, pressButton3, sizeof(pressButton3));
			usart_write_buffer_wait(&usart_instance, '\n', 1);
			for(i = 0; i < sizeof(pressButton3) - 1; i++)
				gfx_mono_draw_char(pressButton3[i], i*SYSFONT_WIDTH, 2*SYSFONT_HEIGHT, &sysfont);
			//delay_cycles_ms(50);															// Não tem necessidade o delay, eu acho
		} else {
			usart_write_buffer_wait(&usart_instance, "None\n", sizeof("None"));
			usart_write_buffer_wait(&usart_instance, '\n', 1);
		}
	} while (true);
}