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

struct Register : Gate
{
  explicit Register()
    : Gate(
        18,                 // 16-bit input, load, clock
        16,                 // 16-bit output 
        GateType::REGISTER, // Gate type
        "register"          // Gate name
      )
  {
  }

  auto load_pin() -> Pin& 
  {
    return this->input_pins[16];
  }

  auto clock_pin() -> Pin& 
  {
    return this->input_pins[17];
  }

  auto handle_register_impl() -> void
  {
    const uint16_t loaded_value = pinvec_to_uint(this->input_pins, 0, 16);

    if (clock_pin().is_active() && load_pin().is_active())
    {
      this->data = loaded_value;

      set_pinvec(loaded_value, this->output_pins, 0, 16);
    }
  }

  /**
   * Members
   */
  uint16_t data;
};

