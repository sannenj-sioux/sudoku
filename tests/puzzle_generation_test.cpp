#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "scene.h"

TEST(PuzzleGenerationTest, GeneratedPuzzleIsCompleteBeforeErase) {
  CScene scene;

  scene.generate();

  EXPECT_TRUE(scene.isComplete());
}

TEST(PuzzleGenerationTest, PuzzleIsNotCompleteAfterErase) {
  CScene scene;

  scene.generate();
  scene.eraseRandomGrids(20);

  EXPECT_FALSE(scene.isComplete());
}
