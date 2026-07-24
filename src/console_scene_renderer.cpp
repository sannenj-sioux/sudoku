#include "console_scene_renderer.h"

#include <iostream>

#include "board.h"
#include "color.h"
#include "display_symbol.h"
#include "utility.inl"

namespace {
void PrintUnderline(const point_t& cursor_point, int max_column, int line_no = -1) {
  const auto is_curline = (cursor_point.y == line_no);
  for (int column = 0; column < max_column; ++column) {
    if ((column % BOX_SIZE) == 0 || line_no == -1 || ((line_no + 1) % BOX_SIZE) == 0) {
      std::cout << Color::Modifier(Color::BOLD, Color::BG_DEFAULT, Color::FG_RED) << CORNER
                << Color::Modifier();
    } else {
      std::cout << CORNER;
    }
    const char* third_symbol = (is_curline && cursor_point.x == column) ? ARROW : LINE;
    if (line_no == -1 || ((line_no + 1) % BOX_SIZE) == 0) {
      std::cout << Color::Modifier(Color::BOLD, Color::BG_DEFAULT, Color::FG_RED) << LINE
                << third_symbol << LINE << Color::Modifier();
    } else {
      std::cout << LINE << third_symbol << LINE;
    }
  }
  std::cout << Color::Modifier(Color::BOLD, Color::BG_DEFAULT, Color::FG_RED) << CORNER
            << Color::Modifier() << '\n';
}
}  // namespace

void ConsoleSceneRenderer::Render(const Board& board, const point_t& cursor_point, int max_column) const {
  ClearScreen();

  PrintUnderline(cursor_point, max_column);

  const auto& row_blocks = board.rowBlocks();

  int highlighted_num = UNSELECTED;
  if (cursor_point.y >= 0 && cursor_point.y < max_column) {
    const CBlock& cursor_block = row_blocks.at(static_cast<size_t>(cursor_point.y));
    highlighted_num = cursor_block.getNumberValue(cursor_point.x);
  }

  for (int row = 0; row < max_column; ++row) {
    const CBlock& block = row_blocks.at(static_cast<size_t>(row));
    if (cursor_point.y == row) {
      block.print(cursor_point.x, highlighted_num);
    } else {
      block.print(-1, highlighted_num);
    }
    PrintUnderline(cursor_point, max_column, row);
  }
}
