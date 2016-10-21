/*******************************************************************************
 * Version: 1.0.0
 *
 * \file   main.c
 * \brief  This application demonstrate basing functionality
 *
 *******************************************************************************
 * <b> (C) Copyright 2016 Silicon Labs, http://www.silabs.com </b>
 *******************************************************************************
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

/*******************************************************************************
 *******************************   INCLUDES   **********************************
 ******************************************************************************/
#include "config.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "em_device.h"
#include "native_gecko.h"
#include "gecko_configuration.h"
#include "gatt_db.h"

#include "InitDevice.h"
#include "bg_dfu.h"
#include "aat.h"

#include "em_gpio.h"
#include "uartdrv.h"

/*******************************************************************************
 ******************************   DEFINES    ***********************************
 ******************************************************************************/
#if TARGET_BOARD == BRD4300AREVA01 || TARGET_BOARD == BRD4300AREVA02 || TARGET_BOARD == BRD4300AREVA03
# define LED0_bank      gpioPortF
# define LED0_pin      	6

# define LED1_bank      gpioPortF
# define LED1_pin      	7
#elif TARGET_BOARD == BRD4301A
# define LED0_bank      gpioPortD
# define LED0_pin      	14

# define LED1_bank      gpioPortD
# define LED1_pin      	15
#else
# error "Incorrect TARGET_BOARD value"
#endif

#define TIMER_LED_BLINK 1

/*******************************************************************************
 *****************************   LOCAL DATA   **********************************
 ******************************************************************************/
 #define MAX_CONNECTIONS 1
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS)];

/* Gecko configuration parameters (see gecko_configuration.h) */
static const gecko_configuration_t config = {
	.config_flags=0,
	//TODO: needs to be disabled for UART???  .sleep.flags=SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
	.bluetooth.max_connections=MAX_CONNECTIONS,
	.bluetooth.heap=bluetooth_stack_heap,
	.bluetooth.heap_size=sizeof(bluetooth_stack_heap),
	.gattdb=&bg_gattdb_data,
};

/* UART1 config/data */
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, rxBufferQueueIO);
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, txBufferQueueIO);

static const UARTDRV_InitUart_t uart_config = {
	.port=USART1,
	.baudRate=115200,
	.portLocationTx=_USART_ROUTELOC0_TXLOC_LOC0,
	.portLocationRx=_USART_ROUTELOC0_RXLOC_LOC0,
	.stopBits=usartStopbits1,
	.parity=usartNoParity,
	.oversampling=usartOVS16,
	.mvdis=false,
	.fcType=uartdrvFlowControlNone,
	.rxQueue=(UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueueIO,
	.txQueue=(UARTDRV_Buffer_FifoQueue_t *)&txBufferQueueIO
};

UARTDRV_HandleData_t uart_handleData;
UARTDRV_Handle_t uart_handle = &uart_handleData;

static uint8_t rxByte;

/*******************************************************************************
 *****************************   LOCAL FUNCTION   ******************************
 ******************************************************************************/
static void hardware_init(void);
static void services_init(void);
static void ledBlink(void);
static void UART_tx_callback(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data,
				UARTDRV_Count_t transferCount);
static void UART_rx_callback(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data,
				UARTDRV_Count_t transferCount);

/*******************************************************************************
 * @brief
 *   Hardware initialization
 * @param
 *   None
 * @return
 *   None
 ******************************************************************************/
static void hardware_init(void)
{

	// configure LED pins as outputs

	GPIO_PinModeSet(LED0_bank, LED0_pin, gpioModePushPull, 1 );
	GPIO_PinModeSet(LED1_bank, LED1_pin, gpioModePushPull, 1 );

	// initial state: both LEDs off
	GPIO_PinOutSet(LED0_bank, LED0_pin);
	GPIO_PinOutSet(LED1_bank, LED1_pin);

	// Init UART1
	UARTDRV_InitUart(uart_handle, &uart_config);
}

/*******************************************************************************
 * @brief
 *   BLE service initialization
 * @param
 *   None
 * @return
 *   None
 ******************************************************************************/
static void services_init(void)
{
}

/*******************************************************************************
 * @brief
 *   LED blink, called from soft timer
 * @param
 *   None
 * @return
 *   None
 ******************************************************************************/
static void ledBlink(void)
{
	static int toggle = 0;

	if(toggle == 0)
	{
		GPIO_PinOutSet(LED0_bank, LED0_pin);
		GPIO_PinOutClear(LED1_bank, LED1_pin);
		toggle = 1;
	}
	else
	{
		GPIO_PinOutClear(LED0_bank, LED0_pin);
		GPIO_PinOutSet(LED1_bank, LED1_pin);
		toggle = 0;
	}
}

static void UART_tx_callback(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data,
				UARTDRV_Count_t transferCount)
{
	static uint32_t txCnt = 0;

	if (transferStatus == ECODE_EMDRV_UARTDRV_OK) {
		txCnt++;
		gecko_cmd_gatt_server_write_attribute_value(gattdb_uart_tx_bytes, 0, 4, (uint8_t *) &txCnt);
	}
}

static void UART_rx_callback(UARTDRV_Handle_t handle, Ecode_t transferStatus, uint8_t *data,
				UARTDRV_Count_t transferCount)
{
	static uint32_t rxCnt = 0;
	static uint8_t txByte = 0;

	if (transferStatus == ECODE_EMDRV_UARTDRV_OK) {
		txByte = *data;
		UARTDRV_Transmit(uart_handle, &txByte, 1 , UART_tx_callback);

		rxCnt++;
		gecko_cmd_gatt_server_write_attribute_value(gattdb_uart_rx_bytes, 0, 4, (uint8_t *) &rxCnt);
	}

	/* RX the next byte */
	UARTDRV_Receive(uart_handle, &rxByte, 1, UART_rx_callback);
}

/*******************************************************************************
 * @brief
 *   Main()
 * @param
 *   None
 * @return
 *   None
 ******************************************************************************/
void main()
{
	// Initialize Blue Gecko module
	enter_DefaultMode_from_RESET();
	gecko_init(&config);

	// Hardware initialization
	hardware_init();

	// Services initialization
	services_init();

	while (1) {
		/* Event pointer for handling events */
		struct gecko_cmd_packet* evt;

		/* Check for stack event. */
		evt = gecko_wait_event();

		/* Handle events */
		switch (BGLIB_MSG_ID(evt->header)) {

		/* This boot event is generated when the system boots up after reset.
		 * Here the system is set to start advertising immediately after boot procedure. */
		case gecko_evt_system_boot_id:
			/* Print banner on UART */
			UARTDRV_Transmit(uart_handle, (uint8_t *) "Hello\r\n", 7, UART_tx_callback);

			/* Start receiving the first byte */
			UARTDRV_Receive(uart_handle, &rxByte, 1, UART_rx_callback);

			/* Set advertising parameters. 100ms advertisement interval. All channels used.
			 * The first two parameters are minimum and maximum advertising interval, both in
			 * units of (milliseconds * 1.6). The third parameter '7' sets advertising on all channels. */
			gecko_cmd_le_gap_set_adv_parameters(160,160,7);

			/* Start general advertising and enable connections. */
			gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);

			/* start 500ms timer for LED blinking */
			gecko_cmd_hardware_set_soft_timer(32768/2,TIMER_LED_BLINK,0);

			break;

		case gecko_evt_le_connection_closed_id:
			/* Restart advertising after client has disconnected */
			gecko_cmd_le_gap_set_mode(le_gap_general_discoverable, le_gap_undirected_connectable);
			break;

		case gecko_evt_hardware_soft_timer_id:
			switch (evt->data.evt_hardware_soft_timer.handle) {
			case TIMER_LED_BLINK:
				ledBlink();
				break;
			default:
				break;
			}
			break;

		default:
			break;
		}
	}
}
