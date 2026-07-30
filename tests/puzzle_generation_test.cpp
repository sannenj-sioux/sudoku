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

void SetEditableCell(Board& board, int row, int column, int value) {
  point_value_t& cell = AccessCell(board, row, column);
  cell.value = value;
  cell.state = State::ERASED;
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
  // Given
  PuzzleGenerator generator;
  Board board;

  // When
  generator.GenerateSolvedBoard(board);

  // Then
  EXPECT_TRUE(IsSolved(board));
}

TEST(PuzzleGeneratorTest, SolvedBoardUsesEachDigitInEveryRowColumnAndBox) {
  // Given
  PuzzleGenerator generator;
  Board board;

  // When
  generator.GenerateSolvedBoard(board);

  // Then
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
  // Given
  PuzzleGenerator generator;
  Board board;

  // When
  generator.GenerateSolvedBoard(board);

  // Then
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      const point_value_t cell = ReadCell(board, row, column);
      ExpectCellValueInRange(board, row, column);
      EXPECT_EQ(cell.state, State::INITED);
    }
  }
}

TEST(PuzzleGeneratorTest, EraseCellsMarksRequestedNumberOfCellsAsErased) {
  // Given
  PuzzleGenerator generator;
  Board board;
  generator.GenerateSolvedBoard(board);

  // When
  generator.EraseCells(board, 20);

  // Then
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
  // Given
  PuzzleGenerator generator;
  Board board;

  constexpr std::array<int, 3> kEraseCounts = {20, 35, 50};
  for (int erase_count : kEraseCounts) {
    // When
    generator.GenerateSolvedBoard(board);

    // Then
    EXPECT_TRUE(IsSolved(board));
    EXPECT_EQ(CountEmptyCells(board), 0);
    EXPECT_EQ(CountErasedCells(board), 0);

    // When
    generator.EraseCells(board, erase_count);

    // Then
    EXPECT_EQ(CountEmptyCells(board), erase_count);
    EXPECT_EQ(CountErasedCells(board), erase_count);
    EXPECT_FALSE(IsSolved(board));
  }
}

TEST(PuzzleGeneratorTest, BoardValidationStateTracksRowConstraintViolation) {
  // Given
  Board board;
  board.reset();
  SetEditableCell(board, 0, 0, 1);
  SetEditableCell(board, 0, 4, 1);

  // When
  board.refreshValidationState();

  // Then
  EXPECT_FALSE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardInvalidRowState");
  EXPECT_TRUE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::ROW));
  EXPECT_TRUE(HasViolation(ReadCell(board, 0, 4).violation, ConstraintViolation::ROW));
  EXPECT_FALSE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::COLUMN));
  EXPECT_FALSE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::BOX));
}

TEST(PuzzleGeneratorTest, BoardValidationStateTracksColumnConstraintViolation) {
  // Given
  Board board;
  board.reset();
  SetEditableCell(board, 0, 0, 2);
  SetEditableCell(board, 4, 0, 2);

  // When
  board.refreshValidationState();

  // Then
  EXPECT_FALSE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardInvalidColumnState");
  EXPECT_TRUE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::COLUMN));
  EXPECT_TRUE(HasViolation(ReadCell(board, 4, 0).violation, ConstraintViolation::COLUMN));
  EXPECT_FALSE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::ROW));
  EXPECT_FALSE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::BOX));
}

TEST(PuzzleGeneratorTest, BoardValidationStateTracksBoxConstraintViolation) {
  // Given
  Board board;
  board.reset();
  SetEditableCell(board, 0, 0, 3);
  SetEditableCell(board, 1, 1, 3);

  // When
  board.refreshValidationState();

  // Then
  EXPECT_FALSE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardInvalidBoxState");
  EXPECT_TRUE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::BOX));
  EXPECT_TRUE(HasViolation(ReadCell(board, 1, 1).violation, ConstraintViolation::BOX));
  EXPECT_FALSE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::ROW));
  EXPECT_FALSE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::COLUMN));
}

TEST(PuzzleGeneratorTest, BoardValidationStateTracksMultipleConstraintViolations) {
  // Given
  Board board;
  board.reset();
  SetEditableCell(board, 0, 0, 4);
  SetEditableCell(board, 0, 1, 4);
  SetEditableCell(board, 1, 0, 4);

  // When
  board.refreshValidationState();

  // Then
  EXPECT_FALSE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardInvalidMixedState");
  EXPECT_TRUE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::ROW));
  EXPECT_TRUE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::COLUMN));
  EXPECT_TRUE(HasViolation(ReadCell(board, 0, 0).violation, ConstraintViolation::BOX));
}
