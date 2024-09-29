#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hub75.pio.h"

#include "../images/images.h"

// Currently this code only supports MATRIX_HEIGHT 32 and MATRIX_HEIGHT_SPLIT 16
// MATRIX_WIDTH could be changed by adjusting value
#define MATRIX_HEIGHT 32
#define MATRIX_WIDTH 128

#define DATA_PIN_START 2
#define NUM_DATA_PIN 6
// PIN_R1 2
// PIN_G1 3
// PIN_B1 4
// PIN_R2 5
// PIN_G2 6
// PIN_B2 7

#define ADDR_PIN_START 8
#define NUM_ADDR_PIN 4  // PIN_E seems unused
// PIN_A 8
// PIN_B 9
// PIN_C 10
// PIN_D 11
// PIN_E 12

#define PIN_CLK   13
#define PIN_LATCH 14
#define PIN_OE    15

#define FRAME_DURATION 3000000  // In microseconds

void draw_frame(uint32_t *frame, PIO pio, uint sm_data, uint sm_row, uint data_prog_offs) {
  for (int j = 0; j < (1 << NUM_ADDR_PIN); j++) {
    for (int bit = 0; bit < 8; bit++) {
      hub75_data_rgb888_set_shift(pio, sm_data, data_prog_offs, bit);
      for (int i = 0; i < MATRIX_WIDTH; i++) {
        pio_sm_put_blocking(pio, sm_data, frame[j * MATRIX_WIDTH + i]);
        pio_sm_put_blocking(pio, sm_data, frame[(j + (1 << NUM_ADDR_PIN)) * MATRIX_WIDTH + i]);
      }
      // Dummy pixel per lane
      pio_sm_put_blocking(pio, sm_data, 0);
      pio_sm_put_blocking(pio, sm_data, 0);
      // SM is finished when it stalls on empty TX FIFO
      hub75_wait_tx_stall(pio, sm_data);
      // Check previous OE pulse is finished
      hub75_wait_tx_stall(pio, sm_row);
      // Latch row data
      pio_sm_put_blocking(pio, sm_row, j | (50u * (1u << bit) << 5));
    }
  }
}


int main() {
  stdio_init_all();

  PIO pio = pio0;
  uint sm_data = 0;
  uint sm_row = 1;

  uint data_prog_offs = pio_add_program(pio, &hub75_data_rgb888_program);
  uint row_prog_offs = pio_add_program(pio, &hub75_row_program);

  hub75_data_rgb888_program_init(pio, sm_data, data_prog_offs, DATA_PIN_START, PIN_CLK);
  hub75_row_program_init(pio, sm_row, row_prog_offs, ADDR_PIN_START, NUM_ADDR_PIN, PIN_LATCH);

  absolute_time_t start_time = get_absolute_time();
  int64_t time_diff = 0;
  uint image_select = 0;
  uint32_t *frame = (uint32_t*)IMAGES[0];
  while (1) {
    absolute_time_t time_before_draw = get_absolute_time();
    draw_frame(frame, pio, sm_data, sm_row, data_prog_offs);
    absolute_time_t time_after_draw = get_absolute_time();
    time_diff += absolute_time_diff_us(time_before_draw, time_after_draw);
    if (time_diff >= FRAME_DURATION) {
      time_diff -= FRAME_DURATION;
      image_select++;
      if (image_select == NUM_IMAGES) {
        image_select = 0;
      }
      frame = (uint32_t *)IMAGES[image_select];
    }
  }

  return 0;
}