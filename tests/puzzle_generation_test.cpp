#include <gtest/gtest.h>

#include <array>
#include <vector>

#include "scene.h"

namespace {

point_value_t ReadCell(const CScene& scene, int row, int column) {
  return scene.getPointValue(point_t{column, row});
}

std::vector<int> ReadRow(const CScene& scene, int row) {
  std::vector<int> values;
  values.reserve(GRID_SIZE);
  for (int column = 0; column < GRID_SIZE; ++column) {
    values.push_back(ReadCell(scene, row, column).value);
  }
  return values;
}

std::vector<int> ReadColumn(const CScene& scene, int column) {
  std::vector<int> values;
  values.reserve(GRID_SIZE);
  for (int row = 0; row < GRID_SIZE; ++row) {
    values.push_back(ReadCell(scene, row, column).value);
  }
  return values;
}

std::vector<int> ReadBox(const CScene& scene, int start_row, int start_column) {
  std::vector<int> values;
  values.reserve(GRID_SIZE);
  for (int row = start_row; row < start_row + BOX_SIZE; ++row) {
    for (int column = start_column; column < start_column + BOX_SIZE; ++column) {
      values.push_back(ReadCell(scene, row, column).value);
    }
  }
  return values;
}

void ExpectContainsAllDigits(const std::vector<int>& values) {
  ASSERT_EQ(values.size(), static_cast<size_t>(GRID_SIZE));

  std::array<bool, GRID_SIZE + 1> seen{};
  for (int value : values) {
    EXPECT_GE(value, 1);
    EXPECT_LE(value, GRID_SIZE);
    ASSERT_FALSE(seen[static_cast<size_t>(value)]);
    seen[static_cast<size_t>(value)] = true;
  }

  for (int value = 1; value <= GRID_SIZE; ++value) {
    EXPECT_TRUE(seen[static_cast<size_t>(value)]);
  }
}

std::array<std::array<int, GRID_SIZE>, GRID_SIZE> SnapshotValues(const CScene& scene) {
  std::array<std::array<int, GRID_SIZE>, GRID_SIZE> values{};
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      values[static_cast<size_t>(row)][static_cast<size_t>(column)] =
          ReadCell(scene, row, column).value;
    }
  }
  return values;
}

int CountEmptyCells(const CScene& scene) {
  int empty_count = 0;
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      if (ReadCell(scene, row, column).value == static_cast<int>(UNSELECTED)) {
        ++empty_count;
      }
    }
  }
  return empty_count;
}

int CountErasedCells(const CScene& scene) {
  int erased_count = 0;
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      if (ReadCell(scene, row, column).state == State::ERASED) {
        ++erased_count;
      }
    }
  }
  return erased_count;
}

}  // namespace

TEST(PuzzleGenerationTest, GeneratedPuzzleIsCompleteBeforeErase) {
  CScene scene;

  scene.generate();

  EXPECT_TRUE(scene.isComplete());
}

TEST(PuzzleGenerationTest, GeneratedPuzzleUsesEachDigitInEveryRowColumnAndBox) {
  CScene scene;

  scene.generate();

  for (int row = 0; row < GRID_SIZE; ++row) {
    ExpectContainsAllDigits(ReadRow(scene, row));
  }

  for (int column = 0; column < GRID_SIZE; ++column) {
    ExpectContainsAllDigits(ReadColumn(scene, column));
  }

  for (int row = 0; row < GRID_SIZE; row += BOX_SIZE) {
    for (int column = 0; column < GRID_SIZE; column += BOX_SIZE) {
      ExpectContainsAllDigits(ReadBox(scene, row, column));
    }
  }
}

TEST(PuzzleGenerationTest, GeneratedPuzzleStartsFilledAndInitialized) {
  CScene scene;

  scene.generate();

  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      const point_value_t cell = ReadCell(scene, row, column);
      EXPECT_GE(cell.value, 1);
      EXPECT_LE(cell.value, GRID_SIZE);
      EXPECT_EQ(cell.state, State::INITED);
    }
  }
}

TEST(PuzzleGenerationTest, EraseRandomGridsMarksRequestedNumberOfCellsAsErased) {
  CScene scene;

  scene.generate();
  scene.eraseRandomGrids(20);

  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      const point_value_t cell = ReadCell(scene, row, column);
      if (cell.state == State::ERASED) {
        EXPECT_EQ(cell.value, static_cast<int>(UNSELECTED));
      }
    }
  }

  EXPECT_EQ(CountErasedCells(scene), 20);
  EXPECT_EQ(CountEmptyCells(scene), 20);
  EXPECT_FALSE(scene.isComplete());
}

TEST(PuzzleGenerationTest, ErasedCellsCanBeRestoredToRecoverTheGeneratedSolution) {
  CScene scene;

  scene.generate();
  const auto original_values = SnapshotValues(scene);
  scene.eraseRandomGrids(20);

  int restored_count = 0;
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int column = 0; column < GRID_SIZE; ++column) {
      const point_t point{column, row};
      if (scene.getPointValue(point).state == State::ERASED) {
        EXPECT_TRUE(scene.setPointValue(
            point, original_values[static_cast<size_t>(row)][static_cast<size_t>(column)]));
        ++restored_count;
      }
    }
  }

  EXPECT_EQ(restored_count, 20);
  EXPECT_TRUE(scene.isComplete());
}

TEST(PuzzleGenerationTest, GenerateCanBeRepeatedOnTheSameSceneForEachDifficulty) {
  CScene scene;

  constexpr std::array<int, 3> kEraseCounts = {20, 35, 50};
  for (int erase_count : kEraseCounts) {
    scene.generate();

    EXPECT_TRUE(scene.isComplete());
    EXPECT_EQ(CountEmptyCells(scene), 0);
    EXPECT_EQ(CountErasedCells(scene), 0);

    scene.eraseRandomGrids(erase_count);

    EXPECT_EQ(CountEmptyCells(scene), erase_count);
    EXPECT_EQ(CountErasedCells(scene), erase_count);
    EXPECT_FALSE(scene.isComplete());
  }
}
