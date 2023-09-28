//******************************************************************************
//* Project :
//* All Rights Reserved 2023 DevMG
//* This program contains proprietary information which is a trade
//* secret of DevMG and/or its affiliates and also is protected as
//* an unpublished work under applicable Copyright laws. Recipient is
//* to retain this program in confidence and is not permitted to use or
//* make copies thereof other than as permitted in a written agreement
//* with DevMG, unless otherwise expressly allowed by applicable laws
//* DevMG - 2 Impasse d'Amsterdam 49460 Montreuil-Juigne
//******************************************************************************

#ifndef DRV_UART_DMA_H_
#define DRV_UART_DMA_H_

#ifdef	__cplusplus
extern "C" {
#endif

// INCLUDES ********************************************************************

#include <stdint.h>

#include "usart.h" // CubeMX generated.

#include "lwrb.h" // For ring buffer tx. Rx is natively managed by circular dma.

// DEFINES *********************************************************************
// TYPEDEFS ********************************************************************

  typedef enum {
    uartEvt_none = 0,
    uartEvt_warningRxFirstHalfOverwrite,  // First half RX may be overwritten.
    uartEvt_warningRxSecondHalfOverwrite, // Second half RX may be overwritten.
    uartEvt_frameErrorRx,                 // Frame error, check RX pullup.
    uartEvt_txOverflow,   // TX buffer overflow.
  } uartEvt_t;

  typedef struct {
    uint8_t u8Num;              // Number of instance [0..MAX_INSTANCES_NB-1].
    UART_HandleTypeDef *pHalUart; // Pointer to HAL USARTx handle.
    uint32_t dmaTxChannel;
    uint32_t dmaRxChannel;
    uartEvt_t event;            // Event.
    // Tx.
    lwrb_t txRingBuf;
    uint16_t u16TxCurSz;        // Size of current transmission.
    // Rx.
    uint8_t *pRxBuf;            // Pointer to rx buffer.
    uint16_t u16RxBufSz;        // Size of rx buffer.
    uint16_t u16RxPos;          // Position in rx buffer.
  } drvUart_t;

// MACROS **********************************************************************
// EXTERNS *********************************************************************
// PROTOTYPES ******************************************************************

  // init / task / instantiate.
  void drvUart_init(void);
  void drvUart_task(void);
  drvUart_t *drvUart_instantiate(UART_HandleTypeDef *pHalUart,
                                 uint8_t *pTxBuf, uint16_t u16TxSz,
                                 uint8_t *pRxBuf, uint16_t u16RxSz);
  // tx.
  void drvUart_tx(drvUart_t *pUart, uint8_t *pBuf, uint16_t u16Sz);
  void drvUart_strTx(drvUart_t *pUart, const char *str);
  // rx.
  void drvUart_rxCallback(drvUart_t *pUart, uint8_t *pu8RxBuf, uint16_t u16RxSz);
  void drvUart_eventCallback(drvUart_t *pUart, uartEvt_t event);

#ifdef	__cplusplus
}
#endif

#endif

// EOF *************************************************************************
