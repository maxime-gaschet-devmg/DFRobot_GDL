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

//******************************************************************************
// Driver / Service: driver for uart, using DMA.
/* Use:
  - Configure USARTx with the correct baudrate, wordLength=8bits, no parity, 1 
  stop bit, receive and transmit, fifo mode disable. Check the RX line has an
  hardware or software pull-up.
  - Add a DMA channel to USARTx_TX, configured memory to peripheral, mode NORMAL,
  data width byte and increment address on Memory only.
  - Add a DMA channel to USARTx_RX, configured memory to peripheral, mode CIRCULAR,
  data width byte and increment address on Memory only.
  - Don't forget to enable USARTx IRQ otherwise, the state of the usart driver
  will stay in busy tx (managed in HAL driver).
  - DMA & UART shoud have the same priority / subpriority.
  - IRQs will follow this sequence after a transmission.
    => DMA IRQ Half: not used in our driver.
    => DMA IRQ Full: not used in our driver.
    => UART IRQ End of tx: not used in our driver but used in HAL driver. */
//******************************************************************************

// INCLUDES ********************************************************************

#include "drvUartDma.h"

#include <string.h>             // For strlen().
#include "stm32l5xx_ll_dma.h"   // LL HAL.

// DEFINES *********************************************************************

// If uartPrintf is not defined, define it here with generic printf.
#ifndef uartPrintf // $TODO: replace with a common dbgPrintf.
  #include "drvRtt.h"
  #define uartPrintf drvRtt_printf
#endif

// Maximum number of driver instances.
#define MAX_INSTANCES_NB (1)

// TYPEDEFS ********************************************************************
// VARIABLES *******************************************************************

static uint8_t u8NbInst;                   // Number of instances used.
static drvUart_t asPool[MAX_INSTANCES_NB]; // Pool of drivers.

// FUNCTIONS *******************************************************************

static drvUart_t* getDrvFromHandle(UART_HandleTypeDef *pHandle);
static void startTxDmaTransfer(drvUart_t *pUart);

//******************************************************************************
// Name: drvUart_init()
// Params: void
// Return: void
// Brief: init of uart driver. This function should be called once right
//  after MCU boot up.
//******************************************************************************
void drvUart_init(void)
{
  // Init driver pool.
  u8NbInst = 0;
  memset(asPool, 0, sizeof(asPool));
  for(int i = 0; i < MAX_INSTANCES_NB; i++) {
    asPool[i].u8Num = i;
  }
}

//******************************************************************************
// Name: drvUart_task()
// Params: void
// Return: void
// Brief: maintain task of uart driver. It checks if bytes were received.
//******************************************************************************
void drvUart_task(void)
{
  static int i;
  static drvUart_t *pDrv;
  static uint16_t newPos;

  for(i = 0; i < u8NbInst; i++) // Iterate through instances.
  {
    pDrv = &asPool[i];

    // Calculate current position in buffer and check for new data available.
    newPos = pDrv->u16RxBufSz - LL_DMA_GetDataLength(pDrv->pHalUart->hdmarx->DmaBaseAddress,
                                                     pDrv->dmaRxChannel);
    if(newPos != pDrv->u16RxPos)    // Check change in received data.
    {
      if(newPos > pDrv->u16RxPos) { // Current position is over previous one.
        drvUart_rxCallback(pDrv, &pDrv->pRxBuf[pDrv->u16RxPos],
                           newPos - pDrv->u16RxPos);
      }
      else
      {
        drvUart_rxCallback(pDrv, &pDrv->pRxBuf[pDrv->u16RxPos],
                           pDrv->u16RxBufSz - pDrv->u16RxPos);
        if(newPos > 0) {
          drvUart_rxCallback(pDrv, pDrv->pRxBuf, newPos);
        }
      }
      pDrv->u16RxPos = newPos; // Save current position as old for next transfers.
    }

    // Check event raised in RX IRQs.
    if(uartEvt_none != pDrv->event) {
      drvUart_eventCallback(pDrv, pDrv->event);
      pDrv->event = uartEvt_none;
    }
  }
}

//******************************************************************************
// Name: drvUart_instantiate()
// Params: pointer to uart driver to be initialized, to HAL uart (USARTx), to tx
//        buffer and its size, to rx buffer and its size.
// Return: void
// Brief: init the uart driver with input parameters.
//******************************************************************************
drvUart_t *drvUart_instantiate(UART_HandleTypeDef *pHalUart,
                               uint8_t *pTxBuf, uint16_t u16TxSz,
                               uint8_t *pRxBuf, uint16_t u16RxSz)
{
  drvUart_t *pDrv = NULL;
  if(u8NbInst < MAX_INSTANCES_NB) // Search for room in pool.
  {
    pDrv = &(asPool[u8NbInst]);   // Retrieve pointer from pool.
    pDrv->pHalUart = pHalUart;    // Init driver.
    pDrv->dmaRxChannel = __LL_DMA_GET_CHANNEL(pHalUart->hdmarx->Instance);
    pDrv->dmaTxChannel = __LL_DMA_GET_CHANNEL(pHalUart->hdmatx->Instance);
    pDrv->event = uartEvt_none;
    // Tx.
    lwrb_init(&pDrv->txRingBuf, pTxBuf, u16TxSz);
    pDrv->u16TxCurSz = 0;
    // Rx.
    pDrv->pRxBuf = pRxBuf;
    pDrv->u16RxBufSz = u16RxSz;
    pDrv->u16RxPos = 0;

    u8NbInst++; // Inc number of instances.

    // Start reception.
    HAL_UART_Receive_DMA(pHalUart, pRxBuf, u16RxSz);
  }
  else {
    uartPrintf("\n error uart instantiate");
    pDrv = NULL; // $TODO: to del.
  }
  return pDrv;
}

//******************************************************************************
// Name: drvUart_tx()
// Params: pointer to uart driver, size of data to transmit.
// Return: void
// Brief: transmit the data in input over the uart. If callback triggered with
//  event uartEvt_txOverflowAdd/New, increase tx buffer size during init.
//******************************************************************************
void drvUart_tx(drvUart_t *pUart, uint8_t *pBuf, uint16_t u16Sz)
{
  DMA_TypeDef *pDma = pUart->pHalUart->hdmatx->DmaBaseAddress;
  uint32_t dmaChannel = pUart->dmaTxChannel;

  // Abort transmission and consume data already sent.
  HAL_UART_AbortTransmit(pUart->pHalUart);
  uint32_t remainSzToTx = LL_DMA_GetDataLength(pDma, dmaChannel);
  lwrb_skip(&pUart->txRingBuf, pUart->u16TxCurSz - remainSzToTx);

  // Check overflow.
  size_t ringBufFreeBytes = lwrb_get_free(&pUart->txRingBuf);
  if(u16Sz > ringBufFreeBytes) {
    u16Sz = ringBufFreeBytes;
    drvUart_eventCallback(pUart, uartEvt_txOverflow);
  }

  // Add new data to ring buffer and start dma.
  lwrb_write(&pUart->txRingBuf, pBuf, u16Sz);
  startTxDmaTransfer(pUart);
}

//******************************************************************************
// Name: drvUart_strTx()
// Params: pointer to uart driver, string to transmit.
// Return: void
// Brief: transmit a string over uart.
//******************************************************************************
void drvUart_strTx(drvUart_t *pUart, const char *str)
{
  drvUart_tx(pUart, (uint8_t *)str, strlen(str));
}

//******************************************************************************
// Name: drvUart_rxCallback()
// Params: pointer to uart driver, string to transmit.
// Return: void
// Brief: called when a chunck of data has been received. Pay attention to the
//  fact that there is no buffer in driver and this callback will be called
//  several times when a frame is received. It depends on the execution speed
//  of drvUart_task() and uart baudrate.
//******************************************************************************
__WEAK void drvUart_rxCallback(drvUart_t *pUart, uint8_t *pu8RxBuf, uint16_t u16RxSz)
{
  uartPrintf("\n uart%d rx, size=%d, data=[ ", pUart->u8Num, u16RxSz);
  while(u16RxSz > 0) {
    uartPrintf("%02X ", *(pu8RxBuf++));
    u16RxSz--;
  }
  uartPrintf("]");
  // $TODO: add a pointer to function to specify callback function.
}

//******************************************************************************
// Name: drvUart_eventCallback()
// Params: pointer to uart driver, event.
// Return: void
// Brief: called when a warning or error on reception has been detected.
//******************************************************************************
__WEAK void drvUart_eventCallback(drvUart_t *pUart, uartEvt_t event)
{
  uartPrintf("\n uart%d event=", pUart->u8Num);
  switch(event) {
    default: uartPrintf("?"); break;
    case uartEvt_warningRxFirstHalfOverwrite: uartPrintf("warningRxFirstHalf"); break;
    case uartEvt_warningRxSecondHalfOverwrite: uartPrintf("warningRxSecondHalf"); break;
    case uartEvt_frameErrorRx: uartPrintf("frameErrorRx"); break;
    case uartEvt_txOverflow: uartPrintf("txOverflow"); break;
  }
}

//******************************************************************************
// PRIVATE *********************************************************************
//******************************************************************************

//******************************************************************************
// Name: getDrvFromHandle()
// Params: pointer to HAL uart handle.
// Return: pointer to uart driver.
// Brief: from an uart handle, retrieves the corresponding uart driver.
//******************************************************************************
drvUart_t* getDrvFromHandle(UART_HandleTypeDef *pHandle)
{
  for(int i = 0; i < u8NbInst; i++)
  {
    if(asPool[i].pHalUart == pHandle) { 
      return &asPool[i];
    }
  }
  return NULL; // Should never happen.
}

//******************************************************************************
// Name: startTxDmaTransfer()
// Params: pointer to driver.
// Return: void
// Brief: start
//******************************************************************************
void startTxDmaTransfer(drvUart_t *pUart)
{
  // Get address in ring buffer to start transmission and save size of data tx
  //  after dma transfer will be completed.
  pUart->u16TxCurSz = lwrb_get_linear_block_read_length(&pUart->txRingBuf);
  if(pUart->u16TxCurSz > 0)
  {
    uint8_t *pBufTx = lwrb_get_linear_block_read_address(&pUart->txRingBuf);
    HAL_UART_Transmit_DMA(pUart->pHalUart, pBufTx, pUart->u16TxCurSz);
  }
}

//******************************************************************************
// CALLBACK ********************************************************************
//******************************************************************************

//******************************************************************************
// Name: HAL_UART_TxHalfCpltCallback()
// Params: pointer to uart handle.
// Return: void
// Brief: called in IRQ when half of frame was sent.
//******************************************************************************
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart) {
  // Not used!
  UNUSED(huart);
}

//******************************************************************************
// Name: HAL_UART_TxCpltCallback()
// Params: pointer to uart handle.
// Return: void
// Brief: called in IRQ when frame to tx was fully sent.
//******************************************************************************
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  // Consume data sent and restart dma.
  drvUart_t *pDrv = getDrvFromHandle(huart);
  lwrb_skip(&pDrv->txRingBuf, pDrv->u16TxCurSz); // Data sent, ignore these.
  startTxDmaTransfer(pDrv); // Start a new DMA transfer.
}

//******************************************************************************
// Name: HAL_UART_RxHalfCpltCallback()
// Params: pointer to uart handle.
// Return: void
// Brief: called in IRQ when next bytes to write in RX frame are the second half.
//******************************************************************************
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
  // Rise warning event if needed.
  drvUart_t *pDrv = getDrvFromHandle(huart);
  if(pDrv->u16RxPos >= pDrv->u16RxBufSz / 2) {
    pDrv->event = uartEvt_warningRxSecondHalfOverwrite;
  }
}

//******************************************************************************
// Name: HAL_UART_RxCpltCallback()
// Params: pointer to uart handle.
// Return: void
// Brief: called in IRQ when next bytes to write in RX frame are the first half.
//******************************************************************************
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  // Rise warning event if needed.
  drvUart_t *pDrv = getDrvFromHandle(huart);
  if(pDrv->u16RxPos < pDrv->u16RxBufSz / 2) {
    pDrv->event = uartEvt_warningRxFirstHalfOverwrite;
  }
}

//******************************************************************************
// Name: HAL_UART_ErrorCallback()
// Params: pointer to uart handle.
// Return: void
// Brief: called in IRQ when UART detected an invalid state on RX line.
// Please check there is an hardware or software pull-up on RX line,
//  otherwise it will trigger a frame error.
//******************************************************************************
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  // Rise error event.
  drvUart_t *pDrv = getDrvFromHandle(huart);
  pDrv->event = uartEvt_frameErrorRx;
}

//******************************************************************************
// DO NOT EDIT *****************************************************************
//******************************************************************************

// EOF *************************************************************************
