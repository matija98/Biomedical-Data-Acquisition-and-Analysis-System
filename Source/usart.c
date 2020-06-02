#include <usart.h>
// RX FIFO buffer
char RX_BUFFER1[BUFSIZE1];
int RX_BUFFER_HEAD1, RX_BUFFER_TAIL1;
uint8_t TxReady;

char RX_BUFFER6[BUFSIZE6];
int RX_BUFFER_HEAD6, RX_BUFFER_TAIL6;
uint8_t TxReady6;

void USART1_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	// Enable peripheral clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	// Map port A pins for alternate function
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // Pin PA9 (USART1_TX), pin PA10 (USART1_RX)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; // GPIO pins defined as alternate function
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; // I/O pins speed (signal rise time)
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // push-pull output
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // activates pullup resistors
	GPIO_Init(GPIOA, &GPIO_InitStruct); // set chosen pins
	USART_OverSampling8Cmd(USART1, ENABLE);
	// Use USART_InitStruct to config USART1 peripheral
	USART_InitStruct.USART_BaudRate = BAUDRATE; // set baudrate from define
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;// 8 data bits
	USART_InitStruct.USART_StopBits = USART_StopBits_1; // 1 stop bit
	USART_InitStruct.USART_Parity = USART_Parity_No; // no parity check
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // no HW control flow
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // enable both character transmit and receive
	USART_Init(USART1, &USART_InitStruct); // set USART1 peripheral
	// Set interrupt triggers for USART1 ISR (but do not enable USART1 interrupts yet)
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);// should be disbled
	USART_ITConfig(USART1, USART_IT_TC, ENABLE); // transmission completed event (for reseting TxReady flag)
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // character received (to trigger buffering of new character)
	RX_BUFFER_HEAD1 = 0; RX_BUFFER_TAIL1 = 0; // clear rx buffer
	TxReady = 1;
	// Prepare NVIC to receive USART1 IRQs
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // configure USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// max. priority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // max. priority
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // enable USART1 interrupt in NVIC
	NVIC_Init(&NVIC_InitStructure); // set NVIC for USART1 IRQ
	// Enable USART1 interrupt generation
	USART_Cmd(USART1, ENABLE);
}

void USART6_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	// Enable peripheral clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	// Map port G pins for alternate function
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_USART6);  
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; // Pin PG9 (USART6_RX), pin PG14 (USART6_TX)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; // GPIO pins defined as alternate function
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // I/O pins speed (signal rise time)
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // push-pull output
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // activates pullup resistors
	GPIO_Init(GPIOG, &GPIO_InitStruct); // set chosen pins
	// Use USART_InitStruct to config USART6 peripheral
	USART_InitStruct.USART_BaudRate = BAUDRATE; // set baudrate from define
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;// 8 data bits
	USART_InitStruct.USART_StopBits = USART_StopBits_1; // 1 stop bit
	USART_InitStruct.USART_Parity = USART_Parity_No; // no parity check
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // no HW control flow
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // enable both character transmit and receive
	USART_Init(USART6, &USART_InitStruct); // set USART6 peripheral
	// Set interrupt triggers for USART6 ISR (but do not enable USART6 interrupts yet)
	USART_ITConfig(USART6, USART_IT_TXE, DISABLE);// should be disbled
	USART_ITConfig(USART6, USART_IT_TC, ENABLE); // transmission completed event (for reseting TxReady flag)
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE); // character received (to trigger buffering of new character)
	TxReady6 = 1; // USART6 is ready to transmit
	RX_BUFFER_HEAD6 = 0; RX_BUFFER_TAIL6 = 0; // clear rx buffer
	// Prepare NVIC to receive USART6 IRQs
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn; // configure USART6 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// max. priority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // max. priority
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // enable USART6 interrupt in NVIC
	NVIC_Init(&NVIC_InitStructure); // set NVIC for USART6 IRQ
	// Enable USART6 interrupt generation
	USART_Cmd(USART6, ENABLE);
}

// USART1 ISR
void USART1_IRQHandler(void){
	static char rx_char;
	static int rx_head;
	// RX event
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		rx_char = USART_ReceiveData(USART1);
		// Check for buffer overrun:
		rx_head = RX_BUFFER_HEAD1 + 1;
		if (rx_head == BUFSIZE1) rx_head = 0;
		if (rx_head != RX_BUFFER_TAIL1){
			// Adding new char will not cause buffer overrun:
			RX_BUFFER1[RX_BUFFER_HEAD1] = rx_char;
			RX_BUFFER_HEAD1 = rx_head; // update head
		}
	}
	if (USART_GetITStatus(USART1, USART_IT_TC) == SET){
		USART_ClearITPendingBit(USART1, USART_IT_TC);
		TxReady = 1;
	}
}

// USART6 ISR
void USART6_IRQHandler(void){
static char rx_char;
static char rx_head;
 // RX event
 if (USART_GetITStatus(USART6, USART_IT_RXNE) == SET)
 {
	USART_ClearITPendingBit(USART6, USART_IT_RXNE);
	rx_char = USART_ReceiveData(USART6);
	// check for buffer overrun:
	rx_head = RX_BUFFER_HEAD6 + 1;
	if (rx_head == BUFSIZE6) rx_head = 0;
	if (rx_head != RX_BUFFER_TAIL6)
 {
 // adding new char will not cause buffer overrun:
		RX_BUFFER6[RX_BUFFER_HEAD6] = rx_char;
		RX_BUFFER_HEAD6 = rx_head; // update head
 }
 }
 if (USART_GetITStatus(USART6, USART_IT_TC) == SET)
 {
	USART_ClearITPendingBit(USART6, USART_IT_TC);
	TxReady6 = 1;
 }
}

void USART1_SendChar(char c)
{
	while(!TxReady);
	USART_SendData(USART1, c);
	TxReady = 0;
}

void USART6_SendChar(char c)
{
	while(!TxReady6);
	USART_SendData(USART6, c);
	TxReady6 = 0;
}

int USART1_Dequeue(char* c)
{
int ret;
	ret = 0;
	*c = 0;
	NVIC_DisableIRQ(USART1_IRQn);
	if (RX_BUFFER_HEAD1 != RX_BUFFER_TAIL1)
	{
		*c = RX_BUFFER1[RX_BUFFER_TAIL1];
		RX_BUFFER_TAIL1++;
		if (RX_BUFFER_TAIL1 == BUFSIZE1) RX_BUFFER_TAIL1 = 0;
		ret = 1;
	}
NVIC_EnableIRQ(USART1_IRQn);
return ret;
}
