/// Copyright 2023 Olle Mattsson <rymdolle@gmail.com>
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.

#include "lcd.hpp"

void LCD::init()
{
  for (int i = 0; i < sizeof(DB); i++) {
    gpio_init(DB[i]);
    gpio_set_dir(DB[i], GPIO_OUT);
    gpio_put(DB[i], 0);
  }

  gpio_init(RS);
  gpio_set_dir(RS, GPIO_OUT);
  gpio_put(RS, 0);

  gpio_init(RW);
  gpio_set_dir(RW, GPIO_OUT);
  gpio_put(RW, 0);

  gpio_init(EN);
  gpio_set_dir(EN, GPIO_OUT);
  gpio_put(EN, 0);

  gpio_init(CS1);
  gpio_init(CS2);
  gpio_set_dir(CS1, GPIO_OUT);
  gpio_set_dir(CS2, GPIO_OUT);
  gpio_put(CS1, 1);
  gpio_put(CS2, 0);

  gpio_init(RST);
  gpio_set_dir(RST, GPIO_OUT);
  gpio_put(RST, 0);
  sleep_ms(10);
  gpio_put(RST, 1);
  sleep_ms(50);
}

void LCD::on()
{
  wait_busy();
  gpio_put(RS, 0);
  gpio_put(RW, 0);
  gpio_put(DB[0], 1);
  gpio_put(DB[1], 1);
  gpio_put(DB[2], 1);
  gpio_put(DB[3], 1);
  gpio_put(DB[4], 1);
  gpio_put(DB[5], 1);
  gpio_put(DB[6], 0);
  gpio_put(DB[7], 0);

  toggle_clock();
}

void LCD::off()
{
  wait_busy();
  gpio_put(RS, 0);
  gpio_put(RW, 0);
  gpio_put(DB[0], 0);
  gpio_put(DB[1], 1);
  gpio_put(DB[2], 1);
  gpio_put(DB[3], 1);
  gpio_put(DB[4], 1);
  gpio_put(DB[5], 1);
  gpio_put(DB[6], 0);
  gpio_put(DB[7], 0);

  toggle_clock();
}

void LCD::clear()
{
  page0();
  set_address(0);
  for (int y = 0; y < kPageHeight; y++) {
    set_page(y);
    for (int x = 0; x < kPageWidth; x++)
      write(0x00);
  }
  page1();
  set_address(0);
  for (int y = 0; y < kPageHeight; y++) {
    set_page(y);
    for (int x = 0; x < kPageWidth; x++)
      write(0x00);
  }
}

void LCD::set_start(uint8_t line)
{
  wait_busy();
  gpio_put(RS, 0);
  gpio_put(RW, 0);
  gpio_put(DB[6], 1);
  gpio_put(DB[7], 1);

  gpio_put(DB[0], line & 0x01);
  gpio_put(DB[1], line & 0x02);
  gpio_put(DB[2], line & 0x04);
  gpio_put(DB[3], line & 0x08);
  gpio_put(DB[4], line & 0x10);
  gpio_put(DB[5], line & 0x20);

  toggle_clock();
}

void LCD::set_page(uint8_t page)
{
  wait_busy();
  gpio_put(RS, 0);
  gpio_put(RW, 0);
  gpio_put(DB[3], 1);
  gpio_put(DB[4], 1);
  gpio_put(DB[5], 1);
  gpio_put(DB[6], 0);
  gpio_put(DB[7], 1);

  gpio_put(DB[0], page & 0x01);
  gpio_put(DB[1], page & 0x02);
  gpio_put(DB[2], page & 0x04);

  toggle_clock();
}

void LCD::set_address(uint8_t address)
{
  wait_busy();
  gpio_put(RS, 0);
  gpio_put(RW, 0);
  gpio_put(DB[6], 1);
  gpio_put(DB[7], 0);

  gpio_put(DB[0], address & 0x01);
  gpio_put(DB[1], address & 0x02);
  gpio_put(DB[2], address & 0x04);
  gpio_put(DB[3], address & 0x08);
  gpio_put(DB[4], address & 0x10);
  gpio_put(DB[5], address & 0x20);

  toggle_clock();
}

void LCD::page0()
{
  gpio_put(CS1, 1);
  gpio_put(CS2, 0);
}

void LCD::page1()
{
  gpio_put(CS1, 0);
  gpio_put(CS2, 1);
}

void LCD::write(uint8_t data)
{
  wait_busy();
  gpio_put(RS, 1);
  gpio_put(RW, 0);
  for (int i = 0; i < sizeof(DB); i++) {
    gpio_put(DB[i], data & 1 > 0 ? 1 : 0);
    data >>= 1;
  }

  toggle_clock();
}

uint8_t LCD::read()
{
  wait_busy();
  gpio_put(RS, 1);
  gpio_put(RW, 1);
  for (int i = 0; i < sizeof(DB); i++)
    gpio_set_dir(DB[i], GPIO_IN);

  gpio_put(EN, 1);
  sleep_us(kClockDelay);

  uint8_t data = 0;
  for (int i = 0; i < sizeof(DB); i++)
    data |= gpio_get(DB[i]) << i;

  gpio_put(EN, 0);
  sleep_us(kClockDelay);

  for (int i = 0; i < sizeof(DB); i++)
    gpio_set_dir(DB[i], GPIO_OUT);

  return data;
}

uint8_t LCD::status()
{
  gpio_put(RS, 0);
  gpio_put(RW, 1);
  gpio_put(DB[0], 0);
  gpio_put(DB[1], 0);
  gpio_put(DB[2], 0);
  gpio_put(DB[3], 0);
  gpio_put(DB[6], 0);

  gpio_set_dir(DB[4], GPIO_IN);
  gpio_set_dir(DB[5], GPIO_IN);
  gpio_set_dir(DB[7], GPIO_IN);

  gpio_put(EN, 1);
  sleep_us(kClockDelay);

  uint8_t status = 0;
  status |= gpio_get(DB[4]) << 4;
  status |= gpio_get(DB[5]) << 5;
  status |= gpio_get(DB[7]) << 7;
  gpio_put(EN, 0);
  sleep_us(kClockDelay);

  gpio_set_dir(DB[4], GPIO_OUT);
  gpio_set_dir(DB[5], GPIO_OUT);
  gpio_set_dir(DB[7], GPIO_OUT);

  return status;
}

void LCD::wait_busy()
{
  gpio_put(RS, 0);
  gpio_put(RW, 1);
  gpio_set_dir(DB[7], GPIO_IN);

  bool busy = false;
  do {
    gpio_put(EN, 1);
    sleep_us(kClockDelay);
    busy = gpio_get(DB[7]);
    gpio_put(EN, 0);
    sleep_us(kClockDelay);
  } while (busy);

  gpio_set_dir(DB[7], GPIO_OUT);
}
