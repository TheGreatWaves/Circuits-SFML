/** 
 * MIT License
 * 
 * Copyright (c) 2023 Ochawin A.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../gate.hpp"

struct Rom32k : Gate 
{
  // NOTE: The input is supposed to be only one 15-bit input bus for address,
  // but here we have extra for testing purposes.
  explicit Rom32k()
    : Gate(
        48,                 // 16-bit input, 15-bit address (read), 15-bit address (write), load, clock
        16,                 // 16-bit output 
        GateType::ROM_32K,  // Gate type
        "rom_32k"           // Gate name
      )
  {
  }

  auto read_in() -> uint16_t
  {
    return pinvec_to_uint<uint16_t>(this->input_pins, 0, 16);
  }

  auto read_address() -> std::size_t
  {
    return pinvec_to_uint<uint16_t>(this->input_pins, 16, 31);
  }

  auto write_address() -> std::size_t
  {
    return pinvec_to_uint<uint16_t>(this->input_pins, 31, 46);
  }

  auto load_pin() -> Pin&
  {
    return this->input_pins[46];
  }

  auto clock_pin() -> Pin&
  {
    return this->input_pins[47];
  }

  auto sync_output() -> void
  {
    const auto value = data[address];
    set_pinvec(value, this->output_pins, 0, 16);
  }

  auto load_value() -> void
  {
    const auto value = read_in();
    const auto address = write_address();
    data[address] = value;
  }

  auto handle_rom_32k_impl() -> void
  {
    // Set the new address
    address = read_address();

    // Load value into address
    if (clock_pin().is_active() && load_pin().is_active())
    {
      load_value();
    }

    // Synchronize output pins
    sync_output();
  }

  /*
   * Members.
   */
    std::size_t address     {0};
    uint16_t    data[32768] {0};
};


