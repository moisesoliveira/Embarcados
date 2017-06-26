#include <asf.h>
#include <conf_demo.h>

void configure_usart(void);

static OLED1_CREATE_INSTANCE(oled1, OLED1_EXT_HEADER);
struct usart_module usart_instance;

void configure_usart(void)
{
	//! [setup_config]
	struct usart_config config_usart;
	//! [setup_config]
	//! [setup_config_defaults]
	usart_get_config_defaults(&config_usart);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_usart.baudrate    = 9600;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	//! [setup_change_config]

	//! [setup_set_config]
	while (usart_init(&usart_instance,
	EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	}
	//! [setup_set_config]

	//! [setup_enable]
	usart_enable(&usart_instance);
	//! [setup_enable]
}

int main (void)
{
	oled1_init(&oled1);
	
	// Configure SERCOM USART for reception from EDBG Virtual COM Port
	//cdc_rx_init(&cdc_usart, &cdc_rx_handler);
	
	system_init();
	gfx_mono_init();
	
	oled1_set_led_state(&oled1, OLED1_LED2_ID, true);
	
	configure_usart();
	
	// Print the about text in an expanding area
	
	char pressButton1[] = "SAMR21_barUp      ";
	char pressButton2[] = "SAMR21_barDown    ";
	char pressButton3[] = "SAMR21_resetEEPROM...";	// Escreve no máximo 21 caracteres por linha
	char secondLine[]   = "Mina seus cabelo e da";
	char thirthLine[]   = "hora, seu corpao viol";
	
	uint8_t i;
	
	/*
	for(i = 0; i < sizeof(secondLine) - 1; i++)
		gfx_mono_draw_char(secondLine[i], i*SYSFONT_WIDTH, 1*SYSFONT_HEIGHT, &sysfont);
	
	for(i = 0; i < sizeof(thirthLine) - 1; i++)
		gfx_mono_draw_char(thirthLine[i], i*SYSFONT_WIDTH, 2*SYSFONT_HEIGHT, &sysfont);
		
	for(i = 0; i < sizeof(secondLine) - 1; i++)
		gfx_mono_draw_char(secondLine[i], i*SYSFONT_WIDTH, 3*SYSFONT_HEIGHT, &sysfont);
	*/
	
	do {
		if(oled1_get_button_state(&oled1, OLED1_BUTTON1_ID)){
			usart_write_buffer_wait(&usart_instance, pressButton1, sizeof(pressButton1));
			usart_write_buffer_wait(&usart_instance, '\n', 1);
			for(i = 0; i < sizeof(pressButton1) - 1; i++)
				gfx_mono_draw_char(pressButton1[i], i*SYSFONT_WIDTH, 0, &sysfont);
			//delay_cycles_ms(50);															// Não tem necessidade o delay, eu acho
		}
		
		if(oled1_get_button_state(&oled1, OLED1_BUTTON2_ID)){
			usart_write_buffer_wait(&usart_instance, pressButton2, sizeof(pressButton2));
			usart_write_buffer_wait(&usart_instance, '\n', 1);
			for(i = 0; i < sizeof(pressButton2) - 1; i++)
				gfx_mono_draw_char(pressButton2[i], i*SYSFONT_WIDTH, 0, &sysfont);
			//delay_cycles_ms(50);															// Não tem necessidade o delay, eu acho
		}
		
		if(oled1_get_button_state(&oled1, OLED1_BUTTON3_ID)){
			usart_write_buffer_wait(&usart_instance, pressButton3, sizeof(pressButton3));
			usart_write_buffer_wait(&usart_instance, '\n', 1);
			for(i = 0; i < sizeof(pressButton3) - 1; i++)
				gfx_mono_draw_char(pressButton3[i], i*SYSFONT_WIDTH, 0, &sysfont);
			//delay_cycles_ms(50);															// Não tem necessidade o delay, eu acho
		}
	} while (true);
}
