
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
#include "configs.h"
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

#if (SAMD || SAMR21)
void SYSCTRL_Handler(void)
{
	if (SYSCTRL->INTFLAG.reg & SYSCTRL_INTFLAG_BOD33DET) {
		SYSCTRL->INTFLAG.reg |= SYSCTRL_INTFLAG_BOD33DET;
		eeprom_emulator_commit_page_buffer();
	}
}
#endif

int main(){
	// setup
	
	//! Instanciação de componentes e configuração da USART
	//@{
	
	system_init();
		
	oled1_init(&oled1);
	cdc_rx_init(&cdc_usart, &cdc_rx_handler);
	
	configure_usart();
	configure_usart_callbacks();
	
	configure_eeprom();
	configure_bod();
	
	system_interrupt_enable_global();
	//@}
	
	// loop
	while(1)
	{
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