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

#include "toolbox.hpp"
#include "board_gui.hpp"
#include "context.hpp"

void ToolBox::place_component()
{
  // Add the component to the sketch.
  Context::instance()->sketch->add_subgate(m_active_component->get_component_name());

  // Move the component to the sketch gui.
	Context::instance()->board->add_component(std::move(m_active_component));

	m_active_component = nullptr;
}

void ToolBox::select_belt()
{
  auto chosen = m_entries.at(m_active_index)->get_name();
  m_active_component = std::make_unique<ComponentGui>(chosen);
}

void ToolBox::revert_mode()
{
  auto& edit_mode = Context::instance()->edit_mode;

  if (edit_mode == Mode::TEXT)
  edit_mode = m_last_mode;
}
