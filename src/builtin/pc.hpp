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

struct PC : Gate 
{
  explicit PC()
    : Gate(
        20,             // 16-bit input, load, inc, reset, clock
        16,             // 16-bit output 
        GateType::PC,   // Gate type
        "pc"            // Gate name
      ),
      register_value(0)
  {
  }


  auto handle_pc_impl()  -> void
  {
    // std::cout << "[" << clock_pin().is_active() << "]\n";
    if (forwardable())
    {
      // std::cout << "===================\n";
      // std::cout << "reset: " << reset_pin().is_active() << '\n';
      // std::cout << "load: " << load_pin().is_active() << '\n';
      // std::cout << "inc: " << inc_pin().is_active() << '\n';
      if (inc_pin().is_active())
      {
      //   std::cout << "increment\n";
        increment();
        clock_pin().set_off();
      }
      else if (reset_pin().is_active())
      {
        reset();
        clock_pin().set_off();
      }
      else if (load_pin().is_active())
      {
      //   std::cout << "Load\n";
        load();
        clock_pin().set_off();
      }

      sync_output();

    }

    // We don't want to continuously forward. 
    // Only forward once, when the signal turned from inactive to active.
    previous_state = pinvec_to_uint(this->input_pins, 16, 20);
  }

  auto sync_output() -> void
  {
    set_pinvec(this->register_value, this->output_pins, 0, 16);
  }

  auto increment() -> void
  {
    this->register_value++;
  }

  auto load() -> void
  {
    this->register_value = pinvec_to_uint<uint16_t>(input_pins, 0, 16);
  }

  auto reset() -> void
  {
    this->register_value = 0;
  }

  auto forwardable() -> bool
  {
    const auto current_state = pinvec_to_uint(this->input_pins, 16, 20);
    return previous_state != current_state && clock_pin().is_active();
  }


  inline auto load_pin() -> Pin&
  {
    return this->input_pins.at(16);
  }

  inline auto inc_pin() -> Pin&
  {
    return this->input_pins.at(17);
  }

  inline auto reset_pin() -> Pin&
  {
    return this->input_pins.at(18);
  }

  inline auto clock_pin() -> Pin&
  {
    return this->input_pins.at(19);
  }
   
  /*
   * 
   */
  uint16_t register_value { 0 };
  uint32_t previous_state { 0 };
}; 



