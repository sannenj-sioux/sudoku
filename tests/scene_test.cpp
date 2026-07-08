#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "i_scene.h"
#include "mocks/mock_puzzle_generator.h"
#include "puzzle_generator.h"
#include "scene.h"

TEST(SceneTest, GenerateDelegatesToPuzzleGenerator) {
  MockPuzzleGenerator mock_generator;
  std::unique_ptr<IScene> scene = std::make_unique<CScene>(3, &mock_generator);
  PuzzleGenerator generator;

  EXPECT_CALL(mock_generator, GenerateSolvedBoard(::testing::_))
      .Times(1)
      .WillOnce(::testing::Invoke(
          [&generator](Board& board) {
            generator.GenerateSolvedBoard(board);
          }));

  scene->generate();
}