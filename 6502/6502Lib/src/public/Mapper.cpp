// Copyright [2020-2022] <tiansongyu>

#include "Mapper.h"

#include "StateIO.h"

Mapper::Mapper(uint8_t prgBanks, uint8_t chrBanks) {
  nPRGBanks = prgBanks;
  nCHRBanks = chrBanks;

  // reset();
}

Mapper::~Mapper() {}  // virtual：经基类指针析构派生 mapper

MIRROR Mapper::mirror() { return MIRROR::HARDWARE; }

bool Mapper::irqState() { return false; }

void Mapper::scanline() {}

void Mapper::SaveState(std::ostream &os) { PutBytes(os, prgRam); }

void Mapper::LoadState(std::istream &is) { GetBytes(is, prgRam); }
