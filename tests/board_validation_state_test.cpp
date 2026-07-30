#include <gtest/gtest.h>

#include <memory>
#include <utility>

#include "board.h"
#include "board_validation_state.h"
#include "common.h"

namespace {

std::size_t ToIndex(int row, int column) {
  return (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
         static_cast<std::size_t>(column);
}

void SetEditableCell(Board& board, int row, int column, int value) {
  point_value_t& cell = board.at(ToIndex(row, column));
  cell.value = value;
  cell.state = State::ERASED;
}

}  // namespace

TEST(BoardValidationStateTest, FactoryCreatesValidStateForNoViolations) {
  // Given
  const ConstraintViolation summary = ConstraintViolation::NONE;

  // When
  const std::unique_ptr<BoardValidationState> state = CreateBoardValidationState(summary);

  // Then
  ASSERT_NE(state, nullptr);
  EXPECT_TRUE(state->IsValid());
  EXPECT_STREQ(state->Name(), "BoardValidState");
  EXPECT_STREQ(state->Message(), "Board is valid");
}

TEST(BoardValidationStateTest, FactoryCreatesRowInvalidState) {
  // Given
  const ConstraintViolation summary = ConstraintViolation::ROW;

  // When
  const std::unique_ptr<BoardValidationState> state = CreateBoardValidationState(summary);

  // Then
  ASSERT_NE(state, nullptr);
  EXPECT_FALSE(state->IsValid());
  EXPECT_STREQ(state->Name(), "BoardInvalidRowState");
  EXPECT_STREQ(state->Message(), "Row constraint violated");
}

TEST(BoardValidationStateTest, FactoryCreatesColumnInvalidState) {
  // Given
  const ConstraintViolation summary = ConstraintViolation::COLUMN;

  // When
  const std::unique_ptr<BoardValidationState> state = CreateBoardValidationState(summary);

  // Then
  ASSERT_NE(state, nullptr);
  EXPECT_FALSE(state->IsValid());
  EXPECT_STREQ(state->Name(), "BoardInvalidColumnState");
  EXPECT_STREQ(state->Message(), "Column constraint violated");
}

TEST(BoardValidationStateTest, FactoryCreatesBoxInvalidState) {
  // Given
  const ConstraintViolation summary = ConstraintViolation::BOX;

  // When
  const std::unique_ptr<BoardValidationState> state = CreateBoardValidationState(summary);

  // Then
  ASSERT_NE(state, nullptr);
  EXPECT_FALSE(state->IsValid());
  EXPECT_STREQ(state->Name(), "BoardInvalidBoxState");
  EXPECT_STREQ(state->Message(), "Box constraint violated");
}

TEST(BoardValidationStateTest, FactoryCreatesMixedInvalidStateForMultipleViolations) {
  // Given
  const ConstraintViolation summary = ConstraintViolation::ROW | ConstraintViolation::COLUMN;

  // When
  const std::unique_ptr<BoardValidationState> state = CreateBoardValidationState(summary);

  // Then
  ASSERT_NE(state, nullptr);
  EXPECT_FALSE(state->IsValid());
  EXPECT_STREQ(state->Name(), "BoardInvalidMixedState");
  EXPECT_STREQ(state->Message(), "Multiple constraints violated");
}

TEST(BoardValidationStateTest, BoardTransitionsFromValidToInvalidAndBackToValid) {
  // Given
  Board board;
  board.reset();
  board.refreshValidationState();

  // Then
  EXPECT_TRUE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardValidState");

  // When
  SetEditableCell(board, 0, 0, 5);
  SetEditableCell(board, 0, 4, 5);
  board.refreshValidationState();

  // Then
  EXPECT_FALSE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardInvalidRowState");

  // When
  SetEditableCell(board, 0, 4, 4);
  board.refreshValidationState();

  // Then
  EXPECT_TRUE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardValidState");
}

TEST(BoardValidationStateTest, BoardTransitionsToMixedStateThenSingleState) {
  // Given
  Board board;
  board.reset();

  // When
  SetEditableCell(board, 0, 0, 7);
  SetEditableCell(board, 0, 4, 7);
  SetEditableCell(board, 4, 0, 7);
  board.refreshValidationState();

  // Then
  EXPECT_FALSE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardInvalidMixedState");

  // When
  SetEditableCell(board, 4, 0, 8);
  board.refreshValidationState();

  // Then
  EXPECT_FALSE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardInvalidRowState");
}

TEST(BoardValidationStateTest, BoardCopyAndMoveOperationsPreserveValidationState) {
  // Given
  Board source;
  source.reset();
  SetEditableCell(source, 0, 0, 6);
  SetEditableCell(source, 0, 4, 6);
  source.refreshValidationState();

  // When
  Board copy_constructed(source);
  Board copy_assigned;
  copy_assigned = source;
  Board move_constructed(std::move(copy_constructed));
  Board move_assigned;
  move_assigned = std::move(copy_assigned);

  // Then
  EXPECT_FALSE(move_constructed.isValidState());
  EXPECT_STREQ(move_constructed.validationState().Name(), "BoardInvalidRowState");
  EXPECT_TRUE(HasViolation(move_constructed.violationSummary(), ConstraintViolation::ROW));

  EXPECT_FALSE(move_assigned.isValidState());
  EXPECT_STREQ(move_assigned.validationState().Name(), "BoardInvalidRowState");
  EXPECT_TRUE(HasViolation(move_assigned.violationSummary(), ConstraintViolation::ROW));
}

TEST(BoardValidationStateTest, BoardSelfAssignmentsAreNoOps) {
  // Given
  Board board;
  board.reset();
  SetEditableCell(board, 0, 0, 3);
  SetEditableCell(board, 0, 5, 3);
  board.refreshValidationState();

  // When
  board = board;
  board = std::move(board);

  // Then
  EXPECT_FALSE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardInvalidRowState");
  EXPECT_TRUE(HasViolation(board.violationSummary(), ConstraintViolation::ROW));
}
