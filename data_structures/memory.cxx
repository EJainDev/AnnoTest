export module cpputils.data_structures:memory;

import std;

namespace cpputils::data_structures {
template <typename T>
class Memory {
 public:
  std::optional<Memory> ofSize(int size) pre(size >= 0) {
    T* new_data = static_cast<T*>(std::malloc(sizeof(T) * size));
    if (!new_data) {
      throw std::bad_alloc();
    }
    return Memory{new_data, size};
  }

 private:
  T* data = nullptr;
  int size = 0;
};

export template <typename T>
std::optional<T*> alloc(int count) pre(count > 0) {
  T* ptr = static_cast<T*>(std::malloc(sizeof(T) * count));
  if (!ptr) {
    return std::nullopt;
  }
  return {ptr};
}
}  // namespace cpputils::data_structures