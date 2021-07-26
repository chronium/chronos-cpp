#include "serial.h"

#include <arch/x86_io.h>

using namespace platform;

#define SERIAL_IO_BASE 0x3F8

void Serial::Init() {

  // no irq's
  io_outb(SERIAL_IO_BASE + 1, 0x00); // Disable all interrupts
  // write the divisor
  io_outb(SERIAL_IO_BASE + 3, 0x80); // Enable DLAB (set baud rate divisor)
  io_outb(SERIAL_IO_BASE + 0, 0x01); // Set divisor to 1 (lo byte) 115200 baud
  io_outb(SERIAL_IO_BASE + 1, 0x00); //                  (hi byte)

  // 8N1
  io_outb(SERIAL_IO_BASE + 3, 0x03); // 8 bits, no parity, one stop bit
  // enable and clear FIFOs. 14 byte threshold
  io_outb(SERIAL_IO_BASE + 2,
          0xC7); // Enable FIFO, clear them, with 14-byte threshold
  io_outb(SERIAL_IO_BASE + 4, 0x0B); // IRQs enabled, RTS/DSR set

  // do a loopback test (write to it)
  io_outb(SERIAL_IO_BASE + 4,
          0x1E); // Set in loopback mode, test the serial chip
  io_outb(SERIAL_IO_BASE + 0, 0xAE); // Test serial chip (send byte 0xAE and
                                     // check if serial returns same byte)

  // expect to read back the same value
  if (io_inb(SERIAL_IO_BASE + 0) != 0xAE) {
    // panic("debug spew port is broken (%04x)", SPEW_IO_BASE);
  }

  // set operation mode
  io_outb(SERIAL_IO_BASE + 4, 0x0F);
}

void Serial::WaitTxRdy() {
  bool empty = false;

  do {
    empty = (io_inb(SERIAL_IO_BASE + 5) & 0x20) ? true : false;
  } while (!empty);
}

void Serial::Tx(char ch) {
  // convert newlines from CR to CR+LF
  if (ch == '\n') {
    io_outb(SERIAL_IO_BASE + 0, '\r');
    Serial::WaitTxRdy();
    io_outb(SERIAL_IO_BASE + 0, ch);
  } else {
    io_outb(SERIAL_IO_BASE + 0, ch);
  }
}

void platform_debug_spew(char ch) {
  Serial::WaitTxRdy();
  Serial::Tx(ch);
}
