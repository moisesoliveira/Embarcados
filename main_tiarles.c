
/*
 * Elementos a Serem inseridos na aferência atual do código:
 * 1º) Mandar o comando do OLED1 para a aplicação via Serial
 *		OLED1_BUTTON1_ID = Barra pra Cima
 *		OLED1_BUTTON2_ID = Barra pra Baixo
 *		OLED1_BUTTON3_ID = reseta o jogo (sem o Ranking)
 *
 */
  
/** 
 * \file 
 *
 * \brief Programa Principal da Aplicação
 *
 */

#include <asf.h>

//void usart_read_callback(struct usart_module *const usart_module);
void usart_write_callback(struct usart_module *const usart_module);

static struct usart_module cdc_usart;


static OLED1_CREATE_INSTANCE(oled1, OLED1_EXT_HEADER);
/*   const struct oled1_instance name = {          \
 *       .led0_pin = ext_header ## _PIN_7,         \
 *       .led1_pin = ext_header ## _PIN_8,         \
 *       .led2_pin = ext_header ## _PIN_6,         \
 *       .button0_pin = ext_header ## _PIN_9,      \
 *       .button1_pin = ext_header ## _PIN_3,      \
 *       .button2_pin = ext_header ## _PIN_4,      \
 (   }
*/

void usart_write_callback(struct usart_module *const usart_module)
{
	port_pin_toggle_output_level(LED_0_PIN);
}

void configure_usart_callbacks(void)
{
	usart_register_callback(&usart_instance,
			usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_instance,
			usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);

	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
}

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

int main(){
	// setup
	
	//! Instanciação de componentes e configuração da USART
	//@{
	
	system_init();
		
	oled1_init(&oled1);
	cdc_rx_init(&cdc_usart, &cdc_rx_handler);
	
	configure_usart();
	configure_usart_callbacks();
	
	system_interrupt_enable_global();
	//@}
	
	// loop
	while(1)
	{
		if()
		
		//! Strings que seram escritas na serial pelo SAMR21_barDown
		//@{
		char upBar_button1[] = "SAMR21_barUp";
		char upBar_button2[] = "SAMR21_barDown";
		char upBar_button3[] = "SAMR21_resetTheGame";
		//@}
		
		//! Obtenção do estado do botão e escrita no serial
		//@{
		if(oled1_get_button_state(&oled1, OLED1_BUTTON1_ID))
			usart_write_buffer_wait(&usart_instance, upBar_button1, sizeof(upBar_button1));
		
		if(oled1_get_button_state(&oled1, OLED1_BUTTON2_ID))
			usart_write_buffer_wait(&usart_instance, upBar_button2, sizeof(upBar_button2));
		
		if(oled1_get_button_state(&oled1, OLED1_BUTTON3_ID))
			usart_write_buffer_wait(&usart_instance, upBar_button3, sizeof(upBar_button3));
		//@}
		
	}

}