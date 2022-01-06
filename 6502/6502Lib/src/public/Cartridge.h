// Copyright [2021] <tiansongyu>
#ifndef _6502_6502LIB_SRC_PUBLIC_CARTRIDGE_H_
#define _6502_6502LIB_SRC_PUBLIC_CARTRIDGE_H_
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "Mapper_000.h"
// 待添加不同种类的mapper
#include "Mapper_001.h"
#include "Mapper_002.h"
#include "Mapper_003.h"
#include "Mapper_004.h"
#include "Mapper_066.h"

class Cartridge {
 public:
  explicit Cartridge(const std::string &sFileName);
  ~Cartridge();

 public:
  bool ImageValid();
  std::shared_ptr<Mapper> GetMapper();

 private:
  bool bImageValid = false;
  MIRROR hw_mirror = HORIZONTAL;

  uint8_t nMapperID = 0;
  uint8_t nPRGBanks = 0;
  uint8_t nCHRBanks = 0;

  std::vector<uint8_t> vPRGMemory;
  std::vector<uint8_t> vCHRMemory;

  std::shared_ptr<Mapper> pMapper;

 public:
  // 与总线通信
  bool cpuRead(uint16_t addr, uint8_t &data);
  bool cpuWrite(uint16_t addr, uint8_t data);

  // 与PPU通信总线
  bool ppuRead(uint16_t addr, uint8_t &data);
  bool ppuWrite(uint16_t addr, uint8_t data);

  // 重置卡带
  void reset();

  MIRROR Mirror();
};
#endif  // _6502_6502LIB_SRC_PUBLIC_CARTRIDGE_H_
