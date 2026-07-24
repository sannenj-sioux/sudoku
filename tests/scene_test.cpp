#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "board.h"
#include "common.h"
#include "i_scene.h"
#include "i_scene_command.h"
#include "key_definitions.h"
#include "mocks/mock_puzzle_generator.h"
#include "mocks/mock_scene_input.h"
#include "mocks/mock_scene_renderer.h"
#include "scene.h"

namespace {
class TestableScene : public CScene {
 public:
  using CScene::CScene;

  void ShowForTest() const { show(); }
};

void FillSolvedBoard(Board& board) {
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int col = 0; col < GRID_SIZE; ++col) {
      const auto index = static_cast<size_t>(row * GRID_SIZE + col);
      board.at(index).value = (row * BOX_SIZE + row / BOX_SIZE + col) % GRID_SIZE + 1;
      board.at(index).state = State::INITED;
    }
  }
}

void ExpectQuitWithoutSave(MockSceneInput& mock_input) {
  EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kEsc));
  EXPECT_CALL(mock_input, ReadToken()).WillOnce(::testing::Return("y"));
  EXPECT_CALL(mock_input, ReadToken()).WillOnce(::testing::Return("n"));
}

void ExpectRenderAt(MockSceneRenderer& mock_renderer, int x, int y) {
  EXPECT_CALL(mock_renderer,
              Render(::testing::_,
                     ::testing::Truly([x, y](const point_t& point) {
                       return point.x == x && point.y == y;
                     }),
                     GRID_SIZE));
}
}  // namespace

TEST(SceneTest, GenerateDelegatesToPuzzleGenerator) {
  MockPuzzleGenerator mock_generator;
  std::unique_ptr<IScene> scene = std::make_unique<CScene>(3, &mock_generator);

  EXPECT_CALL(mock_generator, GenerateSolvedBoard(::testing::_))
      .Times(1)
      .WillOnce(::testing::Invoke([](Board& board) {
        FillSolvedBoard(board);
      }));

  scene->generate();
}

TEST(SceneTest, EraseRandomGridsDelegatesToPuzzleGeneratorWithCount) {
  MockPuzzleGenerator mock_generator;
  std::unique_ptr<IScene> scene = std::make_unique<CScene>(3, &mock_generator);

  EXPECT_CALL(mock_generator, EraseCells(::testing::_, 12)).Times(1);

  scene->eraseRandomGrids(12);
}

TEST(SceneTest, ShowDelegatesToSceneRenderer) {
  MockSceneRenderer mock_renderer;
  TestableScene scene(3, nullptr, &mock_renderer);

  ExpectRenderAt(mock_renderer, 0, 0);

  scene.ShowForTest();
}

TEST(SceneTest, PlayQuitsWhenEscThenConfirmAndNoSave) {
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, nullptr, &mock_renderer, &mock_input);

  ExpectRenderAt(mock_renderer, 0, 0);

  {
    ::testing::InSequence sequence;
    ExpectQuitWithoutSave(mock_input);
  }

  EXPECT_NO_THROW(scene.play());
}

TEST(SceneTest, PlayHandlesExtendedArrowKeyPrefixAndReRenders) {
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, nullptr, &mock_renderer, &mock_input);

  {
    ::testing::InSequence render_sequence;
    ExpectRenderAt(mock_renderer, 0, 0);
    ExpectRenderAt(mock_renderer, 1, 0);
  }

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kExtendedPrefixE0));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kRight));
    ExpectQuitWithoutSave(mock_input);
  }

  EXPECT_NO_THROW(scene.play());
}

TEST(SceneTest, PlayTopLeftBoundaryClamp) {
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, nullptr, &mock_renderer, &mock_input);

  {
    ::testing::InSequence render_sequence;
    ExpectRenderAt(mock_renderer, 0, 0);
    ExpectRenderAt(mock_renderer, 0, 0);
    ExpectRenderAt(mock_renderer, 0, 0);
  }

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kLeft));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kUp));
    ExpectQuitWithoutSave(mock_input);
  }

  EXPECT_NO_THROW(scene.play());
  const auto cursor = scene.getCurPoint();
  EXPECT_EQ(cursor.x, 0);
  EXPECT_EQ(cursor.y, 0);
}

TEST(SceneTest, PlayTopRightBoundaryClamp) {
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, nullptr, &mock_renderer, &mock_input);

  constexpr int kBoundaryPushCount = GRID_SIZE + 2;
  {
    ::testing::InSequence render_sequence;
    ExpectRenderAt(mock_renderer, 0, 0);
    for (int i = 0; i < kBoundaryPushCount; ++i) {
      const int expected_x = (i + 1) > (GRID_SIZE - 1) ? (GRID_SIZE - 1) : (i + 1);
      ExpectRenderAt(mock_renderer, expected_x, 0);
    }
  }

  {
    ::testing::InSequence sequence;
    for (int i = 0; i < kBoundaryPushCount; ++i) {
      EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kRight));
    }
    ExpectQuitWithoutSave(mock_input);
  }

  EXPECT_NO_THROW(scene.play());
  const auto cursor = scene.getCurPoint();
  EXPECT_EQ(cursor.x, GRID_SIZE - 1);
  EXPECT_EQ(cursor.y, 0);
}

TEST(SceneTest, PlayBottomRightBoundaryClamp) {
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, nullptr, &mock_renderer, &mock_input);

  constexpr int kBoundaryPushCount = GRID_SIZE + 2;
  {
    ::testing::InSequence render_sequence;
    ExpectRenderAt(mock_renderer, 0, 0);
    for (int i = 0; i < kBoundaryPushCount; ++i) {
      const int expected_x = (i + 1) > (GRID_SIZE - 1) ? (GRID_SIZE - 1) : (i + 1);
      ExpectRenderAt(mock_renderer, expected_x, 0);
    }
    for (int i = 0; i < kBoundaryPushCount; ++i) {
      const int expected_y = (i + 1) > (GRID_SIZE - 1) ? (GRID_SIZE - 1) : (i + 1);
      ExpectRenderAt(mock_renderer, GRID_SIZE - 1, expected_y);
    }
  }

  {
    ::testing::InSequence sequence;
    for (int i = 0; i < kBoundaryPushCount; ++i) {
      EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kRight));
    }
    for (int i = 0; i < kBoundaryPushCount; ++i) {
      EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kDown));
    }
    ExpectQuitWithoutSave(mock_input);
  }

  EXPECT_NO_THROW(scene.play());
  const auto cursor = scene.getCurPoint();
  EXPECT_EQ(cursor.x, GRID_SIZE - 1);
  EXPECT_EQ(cursor.y, GRID_SIZE - 1);
}

TEST(SceneTest, PlayBottomLeftBoundaryClamp) {
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, nullptr, &mock_renderer, &mock_input);

  constexpr int kBoundaryPushCount = GRID_SIZE + 2;
  {
    ::testing::InSequence render_sequence;
    ExpectRenderAt(mock_renderer, 0, 0);
    for (int i = 0; i < kBoundaryPushCount; ++i) {
      const int expected_y = (i + 1) > (GRID_SIZE - 1) ? (GRID_SIZE - 1) : (i + 1);
      ExpectRenderAt(mock_renderer, 0, expected_y);
    }
    for (int i = 0; i < kBoundaryPushCount; ++i) {
      ExpectRenderAt(mock_renderer, 0, GRID_SIZE - 1);
    }
  }

  {
    ::testing::InSequence sequence;
    for (int i = 0; i < kBoundaryPushCount; ++i) {
      EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kDown));
    }
    for (int i = 0; i < kBoundaryPushCount; ++i) {
      EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kLeft));
    }
    ExpectQuitWithoutSave(mock_input);
  }

  EXPECT_NO_THROW(scene.play());
  const auto cursor = scene.getCurPoint();
  EXPECT_EQ(cursor.x, 0);
  EXPECT_EQ(cursor.y, GRID_SIZE - 1);
}

TEST(SceneTest, PlayValidMovesInAllDirectionsChangeCursor) {
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, nullptr, &mock_renderer, &mock_input);

  {
    ::testing::InSequence render_sequence;
    ExpectRenderAt(mock_renderer, 0, 0);
    ExpectRenderAt(mock_renderer, 1, 0);
    ExpectRenderAt(mock_renderer, 1, 1);
    ExpectRenderAt(mock_renderer, 0, 1);
    ExpectRenderAt(mock_renderer, 0, 0);
  }

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kRight));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kDown));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kLeft));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kUp));
    ExpectQuitWithoutSave(mock_input);
  }

  EXPECT_NO_THROW(scene.play());
}

TEST(SceneTest, PlayReturnsOnEnterWhenBoardIsComplete) {
  MockPuzzleGenerator mock_generator;
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, &mock_generator, &mock_renderer, &mock_input);

  EXPECT_CALL(mock_generator, GenerateSolvedBoard(::testing::_))
      .Times(1)
      .WillOnce(::testing::Invoke([](Board& board) {
        FillSolvedBoard(board);
      }));

  scene.generate();

  ExpectRenderAt(mock_renderer, 0, 0);
  EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kEnter));
  EXPECT_CALL(mock_input, WaitForKey()).Times(1);
  EXPECT_CALL(mock_input, ReadToken()).Times(0);

  EXPECT_NO_THROW(scene.play());
}

TEST(SceneTest, PlayUndoBranchReRendersAfterUndo) {
  MockPuzzleGenerator mock_generator;
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, &mock_generator, &mock_renderer, &mock_input);

  EXPECT_CALL(mock_generator, EraseCells(::testing::_, ::testing::_))
      .Times(1)
      .WillOnce(::testing::Invoke([](Board& board, int) {
        board.at(0).state = State::ERASED;
      }));
  scene.eraseRandomGrids(1);

  {
    ::testing::InSequence render_sequence;
    ExpectRenderAt(mock_renderer, 0, 0);
    ExpectRenderAt(mock_renderer, 0, 0);
    ExpectRenderAt(mock_renderer, 0, 0);
  }

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return('5'));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kUndo));
    ExpectQuitWithoutSave(mock_input);
  }

  EXPECT_NO_THROW(scene.play());
}

TEST(SceneTest, PlayRetriesSavePathUntilSaveSucceeds) {
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, nullptr, &mock_renderer, &mock_input);

  const auto existing_path =
      std::filesystem::temp_directory_path() / "scene_test_play_existing_save_path.sav";
  const auto new_path =
      std::filesystem::temp_directory_path() / "scene_test_play_new_save_path.sav";

  std::filesystem::remove(existing_path);
  std::filesystem::remove(new_path);

  {
    std::ofstream existing_file(existing_path.string());
    existing_file << "already exists";
  }

  ExpectRenderAt(mock_renderer, 0, 0);

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kEsc));
    EXPECT_CALL(mock_input, ReadToken()).WillOnce(::testing::Return("y"));
    EXPECT_CALL(mock_input, ReadToken()).WillOnce(::testing::Return("y"));
    EXPECT_CALL(mock_input, ReadToken()).WillOnce(::testing::Return(existing_path.string()));
    EXPECT_CALL(mock_input, ReadToken()).WillOnce(::testing::Return(new_path.string()));
  }

  EXPECT_NO_THROW(scene.play());
  EXPECT_TRUE(std::filesystem::exists(new_path));

  std::filesystem::remove(existing_path);
  std::filesystem::remove(new_path);
}

TEST(SceneTest, PlayHandlesNonModifiableDigitInputAndContinuesLoop) {
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, nullptr, &mock_renderer, &mock_input);

  ExpectRenderAt(mock_renderer, 0, 0);

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return('5'));
    ExpectQuitWithoutSave(mock_input);
  }

  EXPECT_NO_THROW(scene.play());
}

TEST(SceneTest, PlayCoversIncompleteEnterAndContinuePaths) {
  MockSceneRenderer mock_renderer;
  MockSceneInput mock_input;
  CScene scene(3, nullptr, &mock_renderer, &mock_input);

  {
    ::testing::InSequence render_sequence;
    ExpectRenderAt(mock_renderer, 0, 0);
    ExpectRenderAt(mock_renderer, 0, 0);
    ExpectRenderAt(mock_renderer, 0, 1);
    ExpectRenderAt(mock_renderer, 0, 0);
  }

  {
    ::testing::InSequence sequence;
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kLeft));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kDown));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kUp));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kUndo));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kEnter));
    EXPECT_CALL(mock_input, ReadKey()).WillOnce(::testing::Return(SceneKeys::kEsc));
    EXPECT_CALL(mock_input, ReadToken()).WillOnce(::testing::Return("n"));
    ExpectQuitWithoutSave(mock_input);
  }

  EXPECT_NO_THROW(scene.play());
}

TEST(SceneTest, IsCompleteReturnsFalseForNewScene) {
  CScene scene;

  EXPECT_FALSE(scene.isComplete());
}

TEST(SceneTest, SetPointValueReturnsFalseForInitedCell) {
  CScene scene;

  EXPECT_FALSE(scene.setPointValue({0, 0}, 5));
}

TEST(SceneTest, SetCurValueReturnsFalseForInitedCursorCell) {
  CScene scene;

  int last_value = 99;
  EXPECT_FALSE(scene.setCurValue(5, last_value));
  EXPECT_EQ(last_value, 99);
}

TEST(SceneTest, SetPointValueAndSetCurValueWorkForErasedCell) {
  MockPuzzleGenerator mock_generator;
  CScene scene(3, &mock_generator);

  EXPECT_CALL(mock_generator, EraseCells(::testing::_, ::testing::_))
      .Times(1)
      .WillOnce(::testing::Invoke([](Board& board, int) {
        board.at(0).state = State::ERASED;
      }));

  scene.eraseRandomGrids(1);

  EXPECT_TRUE(scene.setPointValue({0, 0}, 7));
  EXPECT_EQ(scene.getCurPoint().x, 0);
  EXPECT_EQ(scene.getCurPoint().y, 0);

  int last_value = -1;
  EXPECT_TRUE(scene.setCurValue(5, last_value));
  EXPECT_EQ(last_value, 7);
}

TEST(SceneTest, LoadReturnsFalseWhenFileDoesNotExist) {
  CScene scene;
  const auto missing_path =
      std::filesystem::temp_directory_path() / "scene_test_missing_file_should_not_exist.sav";

  std::filesystem::remove(missing_path);

  EXPECT_FALSE(scene.load(missing_path.string().c_str()));
}

TEST(SceneTest, SaveReturnsFalseWhenFileAlreadyExists) {
  CScene scene;
  const auto existing_path =
      std::filesystem::temp_directory_path() / "scene_test_existing_file_should_fail.sav";

  {
    std::ofstream existing_file(existing_path.string());
    existing_file << "already exists";
  }

  EXPECT_FALSE(scene.save(existing_path.string().c_str()));

  std::filesystem::remove(existing_path);
}

TEST(SceneTest, SaveAndLoadRoundTripPreservesCursorAndEditableCellValue) {
  const auto save_path =
      std::filesystem::temp_directory_path() / "scene_test_round_trip_state.sav";
  std::filesystem::remove(save_path);

  MockPuzzleGenerator mock_generator;
  CScene original_scene(3, &mock_generator);

  EXPECT_CALL(mock_generator, EraseCells(::testing::_, ::testing::_))
      .Times(1)
      .WillOnce(::testing::Invoke([](Board& board, int) {
        board.at(0).state = State::ERASED;
      }));

  original_scene.eraseRandomGrids(1);
  EXPECT_TRUE(original_scene.setPointValue({0, 0}, 4));

  int previous_value = -1;
  EXPECT_TRUE(original_scene.setCurValue(6, previous_value));
  EXPECT_EQ(previous_value, 4);

  EXPECT_TRUE(original_scene.save(save_path.string().c_str()));

  CScene loaded_scene;
  EXPECT_TRUE(loaded_scene.load(save_path.string().c_str()));

  const point_t loaded_cursor = loaded_scene.getCurPoint();
  EXPECT_EQ(loaded_cursor.x, 0);
  EXPECT_EQ(loaded_cursor.y, 0);

  int loaded_previous_value = -1;
  EXPECT_TRUE(loaded_scene.setCurValue(8, loaded_previous_value));
  EXPECT_EQ(loaded_previous_value, 6);

  std::filesystem::remove(save_path);
}

TEST(SceneTest, IsCompleteReturnsFalseForFilledButInvalidBoard) {
  MockPuzzleGenerator mock_generator;
  CScene scene(3, &mock_generator);

  EXPECT_CALL(mock_generator, EraseCells(::testing::_, ::testing::_))
      .Times(1)
      .WillOnce(::testing::Invoke([](Board& board, int) {
        FillSolvedBoard(board);
        board.at(1).value = board.at(0).value;
      }));

  scene.eraseRandomGrids(0);

  EXPECT_FALSE(scene.isComplete());
}

TEST(SceneTest, LoadWithCommandHistoryThenSaveSerializesCommands) {
  const auto load_path =
      std::filesystem::temp_directory_path() / "scene_test_load_with_commands_source.sav";
  const auto save_path =
      std::filesystem::temp_directory_path() / "scene_test_load_with_commands_dest.sav";

  std::filesystem::remove(load_path);
  std::filesystem::remove(save_path);

  {
    std::ofstream source(load_path.string());
    for (int i = 0; i < CELL_COUNT; ++i) {
      source << 0 << ' ' << static_cast<int>(State::INITED) << '\n';
    }
    source << 2 << ' ' << 3 << '\n';
    source << 1 << '\n';
    source << 4 << ' ' << 5 << ' ' << 7 << ' ' << 9 << '\n';
  }

  CScene scene;
  EXPECT_TRUE(scene.load(load_path.string().c_str()));
  EXPECT_TRUE(scene.save(save_path.string().c_str()));

  std::ifstream saved(save_path.string());
  std::string line;
  std::string last_non_empty_line;
  while (std::getline(saved, line)) {
    if (!line.empty()) {
      last_non_empty_line = line;
    }
  }
  saved.close();

  EXPECT_EQ(last_non_empty_line, "4 5 7 9");

  std::filesystem::remove(load_path);
  std::filesystem::remove(save_path);
}