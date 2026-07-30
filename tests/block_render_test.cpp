#include <gtest/gtest.h>

#include <string>

#include "block.h"
#include "common.h"

namespace {

constexpr const char* kFgDefault = "\033[1;49;39m";
constexpr const char* kFgLightRed = "\033[1;49;91m";
constexpr const char* kFgLightCyan = "\033[1;49;96m";
constexpr const char* kFgLightMagenta = "\033[1;49;95m";
constexpr const char* kFgWhiteOnRed = "\033[1;41;97m";
constexpr const char* kBgBlue = "\033[1;44;";
constexpr const char* kBgYellow = "\033[1;43;";

CBlock BuildSingleRowBlockWithFirstCell(point_value_t* first_cell) {
  CBlock block;
  block.push_back(first_cell);

  static point_value_t filler_cells[GRID_SIZE - 1];
  for (int i = 0; i < GRID_SIZE - 1; ++i) {
    filler_cells[i] = {static_cast<int>(UNSELECTED), true, ConstraintViolation::NONE};
    block.push_back(&filler_cells[i]);
  }
  return block;
}

}  // namespace

TEST(BlockRenderTest, ProvidedCellWithViolationStaysDefaultColor) {
  // Given
  point_value_t provided_cell{5, true, ConstraintViolation::ROW};
  CBlock block = BuildSingleRowBlockWithFirstCell(&provided_cell);

  // When
  testing::internal::CaptureStdout();
  block.print();
  const std::string output = testing::internal::GetCapturedStdout();

  // Then
  EXPECT_NE(output.find(std::string(kFgDefault) + "5"), std::string::npos);
  EXPECT_EQ(output.find(std::string(kFgLightRed) + "5"), std::string::npos);
}

TEST(BlockRenderTest, EditableCellWithRowViolationIsColoredAsError) {
  // Given
  point_value_t editable_cell{7, false, ConstraintViolation::ROW};
  CBlock block = BuildSingleRowBlockWithFirstCell(&editable_cell);

  // When
  testing::internal::CaptureStdout();
  block.print();
  const std::string output = testing::internal::GetCapturedStdout();

  // Then
  EXPECT_NE(output.find(std::string(kFgLightRed) + "7"), std::string::npos);
}

TEST(BlockRenderTest, EditableCellWithMultipleViolationsUsesMixedErrorStyle) {
  // Given
  point_value_t editable_cell{9,
                              false,
                              ConstraintViolation::ROW | ConstraintViolation::COLUMN};
  CBlock block = BuildSingleRowBlockWithFirstCell(&editable_cell);

  // When
  testing::internal::CaptureStdout();
  block.print();
  const std::string output = testing::internal::GetCapturedStdout();

  // Then
  EXPECT_NE(output.find(std::string(kFgWhiteOnRed) + "9"), std::string::npos);
}

TEST(BlockRenderTest, EditableCellWithColumnViolationIsColoredAsError) {
  // Given
  point_value_t editable_cell{4, false, ConstraintViolation::COLUMN};
  CBlock block = BuildSingleRowBlockWithFirstCell(&editable_cell);

  // When
  testing::internal::CaptureStdout();
  block.print();
  const std::string output = testing::internal::GetCapturedStdout();

  // Then
  EXPECT_NE(output.find(std::string(kFgLightCyan) + "4"), std::string::npos);
}

TEST(BlockRenderTest, EditableCellWithBoxViolationIsColoredAsError) {
  // Given
  point_value_t editable_cell{6, false, ConstraintViolation::BOX};
  CBlock block = BuildSingleRowBlockWithFirstCell(&editable_cell);

  // When
  testing::internal::CaptureStdout();
  block.print();
  const std::string output = testing::internal::GetCapturedStdout();

  // Then
  EXPECT_NE(output.find(std::string(kFgLightMagenta) + "6"), std::string::npos);
}

TEST(BlockRenderTest, EditableCellWithUnknownViolationFallsBackToDefaultColor) {
  // Given
  point_value_t editable_cell{8, false, static_cast<ConstraintViolation>(8)};
  CBlock block = BuildSingleRowBlockWithFirstCell(&editable_cell);

  // When
  testing::internal::CaptureStdout();
  block.print();
  const std::string output = testing::internal::GetCapturedStdout();

  // Then
  EXPECT_NE(output.find(std::string(kFgDefault) + "8"), std::string::npos);
}

TEST(BlockRenderTest, PrintHighlightsCursorAndMatchingNumbers) {
  // Given
  point_value_t cells[GRID_SIZE] = {
      {2, true, ConstraintViolation::NONE},
      {2, true, ConstraintViolation::NONE},
      {3, true, ConstraintViolation::NONE},
      {4, true, ConstraintViolation::NONE},
      {5, true, ConstraintViolation::NONE},
      {6, true, ConstraintViolation::NONE},
      {7, true, ConstraintViolation::NONE},
      {8, true, ConstraintViolation::NONE},
      {9, true, ConstraintViolation::NONE},
  };

  CBlock block;
  for (int i = 0; i < GRID_SIZE; ++i) {
    block.push_back(&cells[i]);
  }

  // When
  testing::internal::CaptureStdout();
  block.print(0, 2);
  const std::string output = testing::internal::GetCapturedStdout();

  // Then
  EXPECT_NE(output.find(kBgBlue), std::string::npos);
  EXPECT_NE(output.find(kBgYellow), std::string::npos);
}

TEST(BlockRenderTest, IsValidHandlesUnselectedAndDuplicateCases) {
  // Given
  point_value_t valid_with_unselected[GRID_SIZE] = {
      {1, true, ConstraintViolation::NONE},
      {2, true, ConstraintViolation::NONE},
      {3, true, ConstraintViolation::NONE},
      {4, true, ConstraintViolation::NONE},
      {5, true, ConstraintViolation::NONE},
      {6, true, ConstraintViolation::NONE},
      {7, true, ConstraintViolation::NONE},
      {8, true, ConstraintViolation::NONE},
      {static_cast<int>(UNSELECTED), false, ConstraintViolation::NONE},
  };

  CBlock block_with_unselected;
  for (int i = 0; i < GRID_SIZE; ++i) {
    block_with_unselected.push_back(&valid_with_unselected[i]);
  }

  point_value_t invalid_with_duplicate[GRID_SIZE] = {
      {1, true, ConstraintViolation::NONE},
      {2, true, ConstraintViolation::NONE},
      {3, true, ConstraintViolation::NONE},
      {4, true, ConstraintViolation::NONE},
      {5, true, ConstraintViolation::NONE},
      {6, true, ConstraintViolation::NONE},
      {7, true, ConstraintViolation::NONE},
      {8, true, ConstraintViolation::NONE},
      {1, false, ConstraintViolation::NONE},
  };

  CBlock block_with_duplicate;
  for (int i = 0; i < GRID_SIZE; ++i) {
    block_with_duplicate.push_back(&invalid_with_duplicate[i]);
  }

  // When + Then
  EXPECT_TRUE(block_with_unselected.isValid());
  EXPECT_FALSE(block_with_duplicate.isValid());
}

TEST(BlockRenderTest, IsFullAndGetNumberValueHandleEdgeCases) {
  // Given
  point_value_t full_cells[GRID_SIZE] = {
      {1, true, ConstraintViolation::NONE},
      {2, true, ConstraintViolation::NONE},
      {3, true, ConstraintViolation::NONE},
      {4, true, ConstraintViolation::NONE},
      {5, true, ConstraintViolation::NONE},
      {6, true, ConstraintViolation::NONE},
      {7, true, ConstraintViolation::NONE},
      {8, true, ConstraintViolation::NONE},
      {9, true, ConstraintViolation::NONE},
  };
  point_value_t not_full_cells[GRID_SIZE] = {
      {1, true, ConstraintViolation::NONE},
      {2, true, ConstraintViolation::NONE},
      {3, true, ConstraintViolation::NONE},
      {4, true, ConstraintViolation::NONE},
      {5, true, ConstraintViolation::NONE},
      {6, true, ConstraintViolation::NONE},
      {7, true, ConstraintViolation::NONE},
      {8, true, ConstraintViolation::NONE},
      {static_cast<int>(UNSELECTED), false, ConstraintViolation::NONE},
  };

  CBlock full_block;
  CBlock not_full_block;
  for (int i = 0; i < GRID_SIZE; ++i) {
    full_block.push_back(&full_cells[i]);
    not_full_block.push_back(&not_full_cells[i]);
  }

  // Then
  EXPECT_TRUE(full_block.isFull());
  EXPECT_FALSE(not_full_block.isFull());
  EXPECT_EQ(full_block.getNumberValue(0), 1);
  EXPECT_EQ(full_block.getNumberValue(GRID_SIZE), static_cast<int>(UNSELECTED));
  EXPECT_EQ(full_block.getNumberValue(-1), static_cast<int>(UNSELECTED));
}

TEST(BlockRenderTest, ErasedZeroCellIsNeverViolationColored) {
  // Given
  point_value_t erased_zero_cell{static_cast<int>(UNSELECTED),
                                 false,
                                 ConstraintViolation::ROW | ConstraintViolation::COLUMN};
  CBlock block = BuildSingleRowBlockWithFirstCell(&erased_zero_cell);

  // When
  testing::internal::CaptureStdout();
  block.print();
  const std::string output = testing::internal::GetCapturedStdout();

  // Then
  EXPECT_EQ(output.find(kFgWhiteOnRed), std::string::npos);
  EXPECT_EQ(output.find(kFgLightRed), std::string::npos);
  EXPECT_EQ(output.find(kFgLightCyan), std::string::npos);
}

TEST(BlockRenderTest, NonGivenFilledCellWithViolationIsErrorColored) {
  // Given
  point_value_t erased_non_zero{5, false, ConstraintViolation::ROW};
  CBlock block = BuildSingleRowBlockWithFirstCell(&erased_non_zero);

  // When
  testing::internal::CaptureStdout();
  block.print();
  const std::string output = testing::internal::GetCapturedStdout();

  // Then
  EXPECT_NE(output.find(kFgLightRed), std::string::npos);
}
