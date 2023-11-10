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

#ifndef LCD_HPP
#define LCD_HPP

#include <string>
#include <cstdint>

#include "pico/stdlib.h"

// JHD12864E LCD
class LCD {
private:
  typedef uint8_t byte;

  byte RS;
  byte RW;
  byte EN;
  byte CS1;
  byte CS2;
  byte RST;
  byte DB[8];

  void toggle_clock() {
    gpio_put(EN, 1);
    sleep_us(kClockDelay);
    gpio_put(EN, 0);
    sleep_us(kClockDelay);
  }

public:
  enum class Status: uint8_t {
    kReset = 1 << 4,
    kOnOff = 1 << 5,
    kBusy = 1 << 7,
  };

  static constexpr int kClockDelay = 4;
  static constexpr int kPageWidth  = 64;
  static constexpr int kPageHeight = 8;
  static constexpr int kDisplayWidth  = kPageWidth * 2;
  static constexpr int kDisplayHeight = 64;

  LCD(byte (&data)[8], byte rs, byte rw, byte en, byte cs1, byte cs2, byte rst) {
    for (int i = 0; i < 8; i++)
      DB[i] = data[i];
    RS=rs;
    RW=rw;
    EN=en;
    CS1=cs1;
    CS2=cs2;
    RST=rst;
  }

  void init();
  void on();
  void off();
  void clear();
  void set_start(byte line);
  void set_page(byte page);
  void set_address(byte address);
  void page0();
  void page1();
  void write(byte data);
  byte read();
  byte status();
  void wait_busy();
};

#endif /* LCD_HPP */
