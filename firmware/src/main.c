#include <asf.h>
#include "conf_board.h"
#include <string.h>
#include "backlit-buttons.h"
#include "abort.h"
#include "throttle.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

// usart (bluetooth ou serial)
// Descomente para enviar dados
// pela serial debug

#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#define USART_COM USART1
#define USART_COM_ID ID_USART1
#else
#define USART_COM USART0
#define USART_COM_ID ID_USART0
#endif

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

#define TASK_BLUETOOTH_STACK_SIZE            (4096/sizeof(portSTACK_TYPE))
#define TASK_BLUETOOTH_STACK_PRIORITY        (tskIDLE_PRIORITY)

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

volatile char estado[] = {0, 0};
volatile int throttle = 0;

/************************************************************************/
/* RTOS application HOOK                                                */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {
	}
}
extern void vApplicationIdleHook(void) {
	pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
}
extern void vApplicationTickHook(void) { }
extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

static void AFEC_pot_callback(void) {
	double temp_throttle = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
	temp_throttle /= 4096;
	temp_throttle *= 127;
	throttle = (throttle * 0.5) + (temp_throttle * 0.5);
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void io_init(void) {
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);
	pmc_enable_periph_clk(BUT4_PIO_ID);
	pmc_enable_periph_clk(BUT5_PIO_ID);
	pmc_enable_periph_clk(BUT6_PIO_ID);
	pmc_enable_periph_clk(BUT7_PIO_ID);
	pmc_enable_periph_clk(BUT8_PIO_ID);
	pmc_enable_periph_clk(BUT9_PIO_ID);
	pmc_enable_periph_clk(BUT10_PIO_ID);
	pmc_enable_periph_clk(ABORT_PIO_ID);
	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT1_PIO, BUT1_IDX_MASK, 100);
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT2_PIO, BUT2_IDX_MASK, 100);
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT3_PIO, BUT3_IDX_MASK, 100);
	pio_configure(BUT4_PIO, PIO_INPUT, BUT4_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT4_PIO, BUT4_IDX_MASK, 100);
	pio_configure(BUT5_PIO, PIO_INPUT, BUT5_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT5_PIO, BUT5_IDX_MASK, 100);
	pio_configure(BUT6_PIO, PIO_INPUT, BUT6_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT6_PIO, BUT6_IDX_MASK, 100);
	pio_configure(BUT7_PIO, PIO_INPUT, BUT7_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT7_PIO, BUT7_IDX_MASK, 100);
	pio_configure(BUT8_PIO, PIO_INPUT, BUT8_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT8_PIO, BUT8_IDX_MASK, 100);
	pio_configure(BUT9_PIO, PIO_INPUT, BUT9_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT9_PIO, BUT9_IDX_MASK, 100);
	pio_configure(BUT10_PIO, PIO_INPUT, BUT10_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT10_PIO, BUT10_IDX_MASK, 100);
	pio_configure(ABORT_PIO, PIO_INPUT, ABORT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(ABORT_PIO, ABORT_IDX_MASK, 100);

	pmc_enable_periph_clk(LED_PIO_ID);
	pio_set_output(LED_PIO, LED_IDX_MASK, 1, 0, 0);
}

void read_but(void) {
	estado[0] = 0 | (!pio_get(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK) << 0);
	estado[0] = estado[0] | (!pio_get(BUT2_PIO, PIO_INPUT, BUT2_IDX_MASK) << 1);
	estado[0] = estado[0] | (!pio_get(BUT3_PIO, PIO_INPUT, BUT3_IDX_MASK) << 2);
	estado[0] = estado[0] | (!pio_get(BUT4_PIO, PIO_INPUT, BUT4_IDX_MASK) << 3);
	estado[0] = estado[0] | (!pio_get(BUT5_PIO, PIO_INPUT, BUT5_IDX_MASK) << 4);
	estado[0] = estado[0] | (!pio_get(BUT6_PIO, PIO_INPUT, BUT6_IDX_MASK) << 5);
	estado[0] = estado[0] | (!pio_get(BUT7_PIO, PIO_INPUT, BUT7_IDX_MASK) << 6);
	estado[1] = 0 | (!pio_get(BUT8_PIO, PIO_INPUT, BUT8_IDX_MASK) << 0);
	estado[1] = estado[1] | (!pio_get(BUT9_PIO, PIO_INPUT, BUT9_IDX_MASK) << 1);
	estado[1] = estado[1] | (!pio_get(BUT10_PIO, PIO_INPUT, BUT10_IDX_MASK) << 2);
	estado[1] = estado[1] | (!pio_get(ABORT_PIO, PIO_INPUT, ABORT_IDX_MASK) << 3);
}

static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback) {
    afec_enable(afec);
    struct afec_config afec_cfg;
    afec_get_config_defaults(&afec_cfg);
    afec_init(afec, &afec_cfg);
    afec_set_trigger(afec, AFEC_TRIG_SW);
    struct afec_ch_config afec_ch_cfg;
    afec_ch_get_config_defaults(&afec_ch_cfg);
    afec_ch_cfg.gain = AFEC_GAINVALUE_0;
    afec_ch_set_config(afec, afec_channel, &afec_ch_cfg);
    afec_channel_set_analog_offset(afec, afec_channel, 0x200);
    struct afec_temp_sensor_config afec_temp_sensor_cfg;
    afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
    afec_temp_sensor_set_config(afec, &afec_temp_sensor_cfg);
    afec_set_callback(afec, afec_channel, callback, 1);
    NVIC_SetPriority(afec_id, 4);
    NVIC_EnableIRQ(afec_id);
}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		#if (defined CONF_UART_CHAR_LENGTH)
		.charlength = CONF_UART_CHAR_LENGTH,
		#endif
		.paritytype = CONF_UART_PARITY,
		#if (defined CONF_UART_STOP_BITS)
		.stopbits = CONF_UART_STOP_BITS,
		#endif
	};
	stdio_serial_init(CONF_UART, &uart_serial_options);
	#if defined(__GNUC__)
	setbuf(stdout, NULL);
	#else
	#endif
}

uint32_t usart_puts(uint8_t *pstring) {
	uint32_t i ;
	while(*(pstring + i))
	if(uart_is_tx_empty(USART_COM))
	usart_serial_putchar(USART_COM, *(pstring+i++));
}

void usart_put_string(Usart *usart, char str[]) {
	usart_serial_write_packet(usart, str, strlen(str));
}

int usart_get_string(Usart *usart, char buffer[], int bufferlen, uint timeout_ms) {
	uint timecounter = timeout_ms;
	uint32_t rx;
	uint32_t counter = 0;
	while( (timecounter > 0) && (counter < bufferlen - 1)) {
		if(usart_read(usart, &rx) == 0) {
			buffer[counter++] = rx;
		}
		else{
			timecounter--;
			vTaskDelay(1);
		}
	}
	buffer[counter] = 0x00;
	return counter;
}

void usart_send_command(Usart *usart, char buffer_rx[], int bufferlen,
char buffer_tx[], int timeout) {
	usart_put_string(usart, buffer_tx);
	usart_get_string(usart, buffer_rx, bufferlen, timeout);
}

void config_usart0(void) {
	sysclk_enable_peripheral_clock(ID_USART0);
	usart_serial_options_t config;
	config.baudrate = 9600;
	config.charlength = US_MR_CHRL_8_BIT;
	config.paritytype = US_MR_PAR_NO;
	config.stopbits = false;
	usart_serial_init(USART0, &config);
	usart_enable_tx(USART0);
	usart_enable_rx(USART0);
	// RX - PB0  TX - PB1
	pio_configure(PIOB, PIO_PERIPH_C, (1 << 0), PIO_DEFAULT);
	pio_configure(PIOB, PIO_PERIPH_C, (1 << 1), PIO_DEFAULT);
}

int hc05_init(void) {
	char buffer_rx[128];
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT+NAMEagoravai", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT+PIN0000", 100);
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

void task_bluetooth(void) {
	printf("Task Bluetooth started \n");
	printf("Inicializando HC05 \n");
	char button1 = '0';
	char eof = -1;
	while(1) {
		read_but();
		// envia status botões e abort
		while(!usart_is_tx_ready(USART_COM)) {
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
		usart_write(USART_COM, estado[0]);
		while(!usart_is_tx_ready(USART_COM)) {
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
		usart_write(USART_COM, estado[1]);
		
		// envia status throttle
		while(!usart_is_tx_ready(USART_COM)) {
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
		usart_write(USART_COM, throttle);
		afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);
		afec_start_software_conversion(AFEC_POT);

		// envia fim de pacote
		while(!usart_is_tx_ready(USART_COM)) {
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
		usart_write(USART_COM, eof);

		// dorme por 10 ms
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void task_read(void) {
	printf("Task Read started \n");
	int i = 0;
	char buffer_rx;
	char buffer_protocol[3];
	pio_clear(LED_PIO,LED_IDX_MASK);
				
	while(1) {
		while(!usart_is_rx_ready(USART_COM)) {
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
		int res = usart_read(USART_COM, &buffer_rx);
		if(!res && (buffer_rx=='\xff')){
			break;
	
		}
	}
	
	while(1) {
				
		while(!usart_is_rx_ready(USART_COM)) {
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
		int res = usart_read(USART_COM, &buffer_rx);
		if(!res){
			printf("%d\n",i);
			buffer_protocol[i] = buffer_rx;
			i++;
		}
		if (i>=3){
			i =0;
			printf("%x %x %x\n",buffer_protocol[0],buffer_protocol[1],buffer_protocol[2]);

			if (buffer_protocol[2]!='\xff') continue;
			printf("%x %x %x\n",buffer_protocol[0],buffer_protocol[1],buffer_protocol[2]);
			
			if (buffer_protocol[0]==1){
				printf("acende");
			
				pio_clear(LED_PIO,LED_IDX_MASK);
	
			} else {
				pio_set(LED_PIO,LED_IDX_MASK);
				
			}
			
			
		}

	}
}
/************************************************************************/
/* main                                                                 */
/************************************************************************/

int main(void) {
	sysclk_init();
	board_init();
	configure_console();
	config_usart0();
	//hc05_init();
	io_init();
	config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, AFEC_pot_callback);
	//xTaskCreate(task_bluetooth, "BLT", TASK_BLUETOOTH_STACK_SIZE, NULL,	TASK_BLUETOOTH_STACK_PRIORITY, NULL);
	xTaskCreate(task_read, "READ", TASK_BLUETOOTH_STACK_SIZE, NULL, TASK_BLUETOOTH_STACK_PRIORITY, NULL);
	vTaskStartScheduler();
	while(1){}
	return 0;
}
