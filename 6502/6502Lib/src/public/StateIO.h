// Copyright [2020-2022] <tiansongyu>
//
// 存档读写的最小工具：平凡可拷贝对象按内存布局原样读写。
// 这不是序列化框架——每个芯片在自己的 SaveState/LoadState 里按
// 字段顺序显式读写。格式与本机字节序/对齐绑定，仅用于本地存档。
#pragma once

#include <cstdint>
#include <istream>
#include <ostream>
#include <type_traits>
#include <vector>

template <typename T>
void PutPod(std::ostream &os, const T &v) {
  static_assert(std::is_trivially_copyable<T>::value,
                "PutPod requires a trivially copyable type");
  os.write(reinterpret_cast<const char *>(&v), sizeof(T));
}

template <typename T>
void GetPod(std::istream &is, T &v) {
  static_assert(std::is_trivially_copyable<T>::value,
                "GetPod requires a trivially copyable type");
  is.read(reinterpret_cast<char *>(&v), sizeof(T));
}

// 容器大小由构造时的硬件配置决定（PRG RAM 8KB、CHR RAM 8KB……），
// 不写入文件，只搬运内容。
inline void PutBytes(std::ostream &os, const std::vector<uint8_t> &v) {
  os.write(reinterpret_cast<const char *>(v.data()),
           static_cast<std::streamsize>(v.size()));
}

inline void GetBytes(std::istream &is, std::vector<uint8_t> &v) {
  is.read(reinterpret_cast<char *>(v.data()),
          static_cast<std::streamsize>(v.size()));
}
