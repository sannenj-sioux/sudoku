#include <gtest/gtest.h>

#include <array>

#include "board.h"
#include "block.h"
#include "puzzle_generator.h"

namespace {

std::size_t ToIndex(int row, int column) {
  return (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
         static_cast<std::size_t>(column);
}

point_value_t ReadCell(const Board& board, int row, int column) {
  return board.at(ToIndex(row, column));
}

point_value_t& AccessCell(Board& board, int row, int column) {
  return board.at(ToIndex(row, column));
}

CBlock BuildRowBlock(Board& board, int row) {
  CBlock block;
  for (int column = 0; column < GRID_SIZE; ++column) {
    block.push_back(&AccessCell(board, row, column));
  }
  return block;
}

CBlock BuildColumnBlock(Board& board, int column) {
  CBlock block;
  for (int row = 0; row < GRID_SIZE; ++row) {
    block.push_back(&AccessCell(board, row, column));
  }
  return block;
}

CBlock BuildBoxBlock(Board& board, int start_row, int start_column) {
  CBlock block;
  for (int row = start_row; row < start_row + BOX_SIZE; ++row) {
    for (int column = start_column; column < start_column + BOX_SIZE; ++column) {
      block.push_back(&AccessCell(board, row, column));
    }
  }
  return block;
}

void ExpectCellValueInRange(const Board& board, int row, int column) {
  const int value = ReadCell(board, row, column).value;
  EXPECT_GE(value, 1);
  EXPECT_LE(value, GRID_SIZE);
}

int CountEmptyCells(const Board& board) {
  int empty_count = 0;
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      if (ReadCell(board, row, column).value == static_cast<int>(UNSELECTED)) {
        ++empty_count;
      }
    }
  }
  return empty_count;
}

int CountErasedCells(const Board& board) {
  int erased_count = 0;
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      if (ReadCell(board, row, column).state == State::ERASED) {
        ++erased_count;
      }
    }
  }
  return erased_count;
}

bool IsSolved(const Board& board) {
  if (CountEmptyCells(board) > 0) {
    return false;
  }

  Board mutable_board = board;

  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      const int value = ReadCell(mutable_board, row, column).value;
      if (value < 1 || value > GRID_SIZE) {
        return false;
      }
    }

    const CBlock row_block = BuildRowBlock(mutable_board, row);
    if (!row_block.isValid() || !row_block.isFull()) {
      return false;
    }
  }

  for (int column = 0; column < GRID_SIZE; ++column) {
    const CBlock column_block = BuildColumnBlock(mutable_board, column);
    if (!column_block.isValid() || !column_block.isFull()) {
      return false;
    }
  }

  for (int start_row = 0; start_row < GRID_SIZE; start_row += BOX_SIZE) {
    for (int start_column = 0; start_column < GRID_SIZE; start_column += BOX_SIZE) {
      const CBlock box_block = BuildBoxBlock(mutable_board, start_row, start_column);
      if (!box_block.isValid() || !box_block.isFull()) {
        return false;
      }
    }
  }

  return true;
}

}  // namespace

TEST(PuzzleGeneratorTest, GenerateSolvedBoardProducesCompleteSudoku) {
  PuzzleGenerator generator;
  Board board;
  generator.GenerateSolvedBoard(board);

  EXPECT_TRUE(IsSolved(board));
}

TEST(PuzzleGeneratorTest, SolvedBoardUsesEachDigitInEveryRowColumnAndBox) {
  PuzzleGenerator generator;
  Board board;
  generator.GenerateSolvedBoard(board);

  for (int row = 0; row < GRID_SIZE; ++row) {
    const CBlock row_block = BuildRowBlock(board, row);
    EXPECT_TRUE(row_block.isValid());
    EXPECT_TRUE(row_block.isFull());
  }

  for (int column = 0; column < GRID_SIZE; ++column) {
    const CBlock column_block = BuildColumnBlock(board, column);
    EXPECT_TRUE(column_block.isValid());
    EXPECT_TRUE(column_block.isFull());
  }

  for (int row = 0; row < GRID_SIZE; row += BOX_SIZE) {
    for (int column = 0; column < GRID_SIZE; column += BOX_SIZE) {
      const CBlock box_block = BuildBoxBlock(board, row, column);
      EXPECT_TRUE(box_block.isValid());
      EXPECT_TRUE(box_block.isFull());
    }
  }
}

TEST(PuzzleGeneratorTest, SolvedBoardStartsFilledAndInitialized) {
  PuzzleGenerator generator;
  Board board;
  generator.GenerateSolvedBoard(board);

  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      const point_value_t cell = ReadCell(board, row, column);
      ExpectCellValueInRange(board, row, column);
      EXPECT_EQ(cell.state, State::INITED);
    }
  }
}

TEST(PuzzleGeneratorTest, EraseCellsMarksRequestedNumberOfCellsAsErased) {
  PuzzleGenerator generator;
  Board board;
  generator.GenerateSolvedBoard(board);
  generator.EraseCells(board, 20);

  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      const point_value_t cell = ReadCell(board, row, column);
      if (cell.state == State::ERASED) {
        EXPECT_EQ(cell.value, static_cast<int>(UNSELECTED));
      }
    }
  }

  EXPECT_EQ(CountErasedCells(board), 20);
  EXPECT_EQ(CountEmptyCells(board), 20);
  EXPECT_FALSE(IsSolved(board));
}

TEST(PuzzleGeneratorTest, GenerateCanBeRepeatedForEachDifficulty) {
  PuzzleGenerator generator;
  Board board;

  constexpr std::array<int, 3> kEraseCounts = {20, 35, 50};
  for (int erase_count : kEraseCounts) {
    generator.GenerateSolvedBoard(board);

    EXPECT_TRUE(IsSolved(board));
    EXPECT_EQ(CountEmptyCells(board), 0);
    EXPECT_EQ(CountErasedCells(board), 0);

    generator.EraseCells(board, erase_count);

    EXPECT_EQ(CountEmptyCells(board), erase_count);
    EXPECT_EQ(CountErasedCells(board), erase_count);
    EXPECT_FALSE(IsSolved(board));
  }
}
