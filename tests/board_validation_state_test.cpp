#include <gtest/gtest.h>

#include <utility>

#include "board.h"
#include "board_validation_state.h"
#include "cell_validation_state_machine.h"
#include "common.h"

namespace {

std::size_t ToIndex(int row, int column) {
  return (static_cast<std::size_t>(row) * static_cast<std::size_t>(GRID_SIZE)) +
         static_cast<std::size_t>(column);
}

void SetEditableCell(Board& board, int row, int column, int value) {
  point_value_t& cell = board.at(ToIndex(row, column));
  cell.value = value;
  cell.is_given = false;
}

}  // namespace

TEST(BoardValidationStateTest, FactoryCreatesValidStateForNoViolations) {
  // Given
  const ConstraintViolation summary = ConstraintViolation::NONE;

  // When
  const BoardValidationState state = CreateBoardValidationState(summary);

  // Then
  EXPECT_TRUE(state.IsValid());
  EXPECT_STREQ(state.Name(), "BoardValidState");
  EXPECT_STREQ(state.Message(), "Board is valid");
}

TEST(BoardValidationStateTest, FactoryCreatesRowInvalidState) {
  // Given
  const ConstraintViolation summary = ConstraintViolation::ROW;

  // When
  const BoardValidationState state = CreateBoardValidationState(summary);

  // Then
  EXPECT_FALSE(state.IsValid());
  EXPECT_STREQ(state.Name(), "BoardInvalidRowState");
  EXPECT_STREQ(state.Message(), "Row constraint violated");
}

TEST(BoardValidationStateTest, FactoryCreatesColumnInvalidState) {
  // Given
  const ConstraintViolation summary = ConstraintViolation::COLUMN;

  // When
  const BoardValidationState state = CreateBoardValidationState(summary);

  // Then
  EXPECT_FALSE(state.IsValid());
  EXPECT_STREQ(state.Name(), "BoardInvalidColumnState");
  EXPECT_STREQ(state.Message(), "Column constraint violated");
}

TEST(BoardValidationStateTest, FactoryCreatesBoxInvalidState) {
  // Given
  const ConstraintViolation summary = ConstraintViolation::BOX;

  // When
  const BoardValidationState state = CreateBoardValidationState(summary);

  // Then
  EXPECT_FALSE(state.IsValid());
  EXPECT_STREQ(state.Name(), "BoardInvalidBoxState");
  EXPECT_STREQ(state.Message(), "Box constraint violated");
}

TEST(BoardValidationStateTest, FactoryCreatesMixedInvalidStateForMultipleViolations) {
  // Given
  const ConstraintViolation summary = ConstraintViolation::ROW | ConstraintViolation::COLUMN;

  // When
  const BoardValidationState state = CreateBoardValidationState(summary);

  // Then
  EXPECT_FALSE(state.IsValid());
  EXPECT_STREQ(state.Name(), "BoardInvalidMixedState");
  EXPECT_STREQ(state.Message(), "Multiple constraints violated");
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

TEST(BoardValidationStateTest, ErasedZeroCellsDoNotAffectValidationState) {
  // Given
  Board board;
  board.reset();

  point_value_t& a = board.at(ToIndex(0, 0));
  point_value_t& b = board.at(ToIndex(0, 1));
  point_value_t& c = board.at(ToIndex(1, 0));
  a.is_given = false;
  b.is_given = false;
  c.is_given = false;
  a.value = static_cast<int>(UNSELECTED);
  b.value = static_cast<int>(UNSELECTED);
  c.value = static_cast<int>(UNSELECTED);

  // When
  board.refreshValidationState();

  // Then
  EXPECT_TRUE(board.isValidState());
  EXPECT_STREQ(board.validationState().Name(), "BoardValidState");
  EXPECT_EQ(a.violation, ConstraintViolation::NONE);
  EXPECT_EQ(b.violation, ConstraintViolation::NONE);
  EXPECT_EQ(c.violation, ConstraintViolation::NONE);
}

TEST(BoardValidationStateTest, CellStateMachineTransitionsAcrossBaseStates) {
  // Given
  CellValidationStateMachine machine;

  // When
  machine.OnCellValueUpdated(true, 4, ConstraintViolation::ROW);

  // Then
  EXPECT_TRUE(machine.IsGiven());
  EXPECT_FALSE(machine.IsErased());
  EXPECT_FALSE(machine.IsUserValue());
  EXPECT_EQ(machine.Violation(), ConstraintViolation::NONE);

  // When
  machine.OnCellValueUpdated(false,
                             static_cast<int>(UNSELECTED),
                             ConstraintViolation::ROW);

  // Then
  EXPECT_FALSE(machine.IsGiven());
  EXPECT_TRUE(machine.IsErased());
  EXPECT_FALSE(machine.IsUserValue());
  EXPECT_EQ(machine.Violation(), ConstraintViolation::NONE);

  // When
  machine.OnCellValueUpdated(false, 5, ConstraintViolation::NONE);

  // Then
  EXPECT_FALSE(machine.IsGiven());
  EXPECT_FALSE(machine.IsErased());
  EXPECT_TRUE(machine.IsUserValue());
  EXPECT_EQ(machine.Violation(), ConstraintViolation::NONE);
}

TEST(BoardValidationStateTest, CellStateMachineTransitionsAcrossUserValueErrorSubstates) {
  // Given
  CellValidationStateMachine machine;

  // When
  machine.OnCellValueUpdated(false, 3, ConstraintViolation::ROW);

  // Then
  EXPECT_TRUE(machine.IsUserValue());
  EXPECT_EQ(machine.Violation(), ConstraintViolation::ROW);

  // When
  machine.OnCellValueUpdated(false, 3, ConstraintViolation::COLUMN);

  // Then
  EXPECT_EQ(machine.Violation(), ConstraintViolation::COLUMN);

  // When
  machine.OnCellValueUpdated(false, 3, ConstraintViolation::BOX);

  // Then
  EXPECT_EQ(machine.Violation(), ConstraintViolation::BOX);

  // When
  machine.OnCellValueUpdated(false,
                             3,
                             ConstraintViolation::ROW | ConstraintViolation::COLUMN);

  // Then
  EXPECT_TRUE(HasViolation(machine.Violation(), ConstraintViolation::ROW));
  EXPECT_TRUE(HasViolation(machine.Violation(), ConstraintViolation::COLUMN));
  EXPECT_TRUE(HasViolation(machine.Violation(), ConstraintViolation::BOX));
}
