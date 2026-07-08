#pragma once

class IScene {
 public:
  virtual ~IScene() = default;

  virtual void generate() = 0;
  virtual void eraseRandomGrids(int count) = 0;
  virtual bool isComplete() = 0;
  virtual void play() = 0;
  virtual bool save(const char* filename) = 0;
  virtual bool load(const char* filename) = 0;
};
