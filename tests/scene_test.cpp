#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mock_puzzle_generator.h"
#include "puzzle_generator.h"
#include "scene.h"

TEST(SceneTest, GenerateDelegatesToPuzzleGenerator) {
  MockPuzzleGenerator mock_generator;
  CScene scene(3, &mock_generator);
  PuzzleGenerator generator;

  EXPECT_CALL(mock_generator, GenerateSolvedBoard(::testing::_))
      .Times(1)
      .WillOnce(::testing::Invoke(
          [&generator](Board& board) {
            generator.GenerateSolvedBoard(board);
          }));

  scene.generate();
}