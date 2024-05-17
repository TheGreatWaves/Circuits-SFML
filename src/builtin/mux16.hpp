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

/**
 * Technically this could just be serialized because it has no internal state,
 * but it takes too long, so implementing it as a built-in would be better. (Unless we seriazlize gates as file)
 */
struct Mux16 : Gate 
{
  explicit Mux16()
    : Gate(
        33,                 // Two 16-bit input, selector
        16,                 // 16-bit output 
        GateType::MUX_16,   // Gate type
        "mux_16"            // Gate name
      )
  {
  }

  auto sel_pin() -> Pin&
  {
    return this->input_pins[32];
  }

  auto handle_mux_16_impl() -> void
  {
    std::size_t start{0}, end{16};


    if (sel_pin().is_active())
    {
      start += 16;
      end += 16;
    }

    const uint16_t value = pinvec_to_uint<uint16_t>(this->input_pins, start, end);
    set_pinvec(value, this->output_pins, 0, 16);
  }
};

