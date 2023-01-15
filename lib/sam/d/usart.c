#include <libopencm3/sam/d/usart.h>

/**
 * It appears that the usart gives most accurate baud rates on a 48MHz/7 fref
 * with 3x arithmetic sampling. Use the 3x arithmetic sampling equation to find BAUD
 * register value. Ensure USART uses 48MHz/7 ref clock.
 **/
void usart_set_baudrate(uint32_t sercom, uint32_t baud) {
	USART_BAUD(sercom) = 65536 - ((65536 * 3 * 7 * baud) / 48000000);
}

void usart_set_chsize(uint32_t sercom, enum usart_chsize chsize)
{
	USART_CTRLB(sercom) = (USART_CTRLB(sercom) & ~USART_CTRLB_CHSIZE_MASK) | USART_CTRLB_CHSIZE(chsize);
}

void usart_set_databits(uint32_t sercom, uint_fast8_t databits) {
	switch (databits) {
    case 5: usart_set_chsize(sercom, USART_CHSIZE_5BIT); break;
    case 6: usart_set_chsize(sercom, USART_CHSIZE_6BIT); break;
    case 7: usart_set_chsize(sercom, USART_CHSIZE_7BIT); break;
    case 8: usart_set_chsize(sercom, USART_CHSIZE_8BIT); break;
    case 9: usart_set_chsize(sercom, USART_CHSIZE_9BIT); break;
    default: break; // Unsupported databit size
  }
}


void usart_set_stopbits(uint32_t sercom, enum usart_stopbits stopbits) {
	USART_CTRLB(sercom) = (USART_CTRLB(sercom) & ~USART_CTRLB_SBMODE_MASK) | USART_CTRLB_SBMODE(stopbits);
}


void usart_set_parity(uint32_t sercom, enum usart_parity parity) {
  switch (parity) {
   case USART_PARITY_EVEN:
    USART_CTRLB(sercom) &= ~USART_CTRLB_PARITY_ODD;
	  USART_CTRLA(sercom) |= USART_CTRLA_HAS_PARITY;
    break;
   case USART_PARITY_ODD:
    USART_CTRLB(sercom) |= USART_CTRLB_PARITY_ODD;
	  USART_CTRLA(sercom) |= USART_CTRLA_HAS_PARITY;
    break;
   case USART_PARITY_NONE:
		USART_CTRLA(sercom) &= ~(USART_CTRLA_HAS_PARITY);
		break;
   default: break;
  }
}


void usart_set_mode(uint32_t sercom, enum usart_mode mode) {
  switch (mode) {
    case USART_MODE_DISABLED:
      USART_CTRLB(sercom) &= ~(USART_CTRLB_RXEN | USART_CTRLB_TXEN);
      break;
	  case USART_MODE_RX:
	    USART_CTRLB(sercom) |= USART_CTRLB_RXEN;
	    break;
	  case USART_MODE_TX:
	    USART_CTRLB(sercom) |= USART_CTRLB_TXEN;
	    break;
	  case USART_MODE_TX_RX:
	    USART_CTRLB(sercom) |= USART_CTRLB_RXEN | USART_CTRLB_TXEN;
	    break;
  }
}

void usart_set_pads(uint32_t sercom, enum usart_tx_pad tx, enum usart_rx_pad rx) {
  USART_CTRLA(sercom) = (USART_CTRLA(sercom) & ~USART_CTRLA_PADS_MASK) | USART_CTRLA_TXPO(tx) | USART_CTRLA_RXPO(rx);
}

void usart_enable(uint32_t sercom) {
  USART_CTRLA(sercom) |= USART_CTRLA_ENABLE;
}


void usart_disable(uint32_t sercom) {
  USART_CTRLA(sercom) &= ~USART_CTRLA_ENABLE;
}


void usart_send(uint32_t sercom, uint16_t data) {

}


uint16_t usart_recv(uint32_t sercom) {

}


void usart_wait_send_ready(uint32_t sercom) {

}


void usart_wait_recv_ready(uint32_t sercom) {

}


void usart_send_blocking(uint32_t sercom, uint16_t data) {

}


uint16_t usart_recv_blocking(uint32_t sercom) {

}


void usart_enable_rx_interrupt(uint32_t sercom) {

}


void usart_disable_rx_interrupt(uint32_t sercom) {

}


void usart_wp_disable(uint32_t sercom) {

}


void usart_wp_enable(uint32_t sercom) {

}


void usart_select_clock(uint32_t sercom, enum usart_clock clk) {

}

