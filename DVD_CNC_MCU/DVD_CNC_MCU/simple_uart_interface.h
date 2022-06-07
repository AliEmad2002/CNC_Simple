#ifndef SIMPLE_UART_INTERFACE_H_
#define SIMPLE_UART_INTERFACE_H_

void UART_init(long USART_BAUDRATE) ;

unsigned char UART_RxChar() ;

void UART_TxChar(char ch) ;

void UART_SendString(char *str) ;



#endif /* SIMPLE_UART_INTERFACE_H_ */