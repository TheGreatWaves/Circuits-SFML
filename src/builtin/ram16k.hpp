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

struct Ram16k : Gate 
{
  explicit Ram16k()
    : Gate(
        32,                 // 16-bit input, 14-bit address, load, clock
        16,                 // 16-bit output 
        GateType::RAM_16K,  // Gate type
        "ram_16k"           // Gate name
      )
  {
  }

  auto read_in() -> uint16_t
  {
    return pinvec_to_uint<uint16_t>(this->input_pins, 0, 16);
  }

  auto read_address() -> std::size_t
  {
    return pinvec_to_uint<uint16_t>(this->input_pins, 16, 30);
  }

  auto load_pin() -> Pin&
  {
    return this->input_pins[30];
  }

  auto clock_pin() -> Pin&
  {
    return this->input_pins[31];
  }

  auto sync_output() -> void
  {
    const auto value = data[address];
    set_pinvec(value, this->output_pins, 0, 16);
  }

  auto load_value() -> void
  {
    const auto value = read_in();
    data[address] = value;
    // std::cout << "LOAD[" << address << "] " << value << '\n';
  }

  auto handle_ram_16k_impl() -> void
  {
    // std::cout << "Clock[" << clock_pin().is_active() << "], Load[" << load_pin().is_active() << "]\n";
    // Set the new address
    address = read_address();


    // Load value into address
    if (!immutable && clock_pin().is_active() && load_pin().is_active())
    {
      load_value();
    }

    // State can ONLY be written ONCE per clock cycle.
    immutable = clock_pin().is_active();

    // Synchronize output pins
    sync_output();
  }

  /*
   * Members.
   */
    std::size_t address     {0};
    uint16_t    data[16384] {0};
    bool immutable { false };
};

