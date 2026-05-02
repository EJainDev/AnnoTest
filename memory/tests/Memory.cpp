import cpputils.testing;
import cpputils.memory;

import std;

template <typename T>
struct Allocator {
  struct Data {
    std::size_t alloc_size;
    std::size_t dealloc_size;
  };

  std::shared_ptr<Data> data = std::make_shared<Data>();

  std::optional<T*> alloc(std::size_t size) {
    data->alloc_size += size;
    return new T[size / sizeof(T)];
  }

  void dealloc(T* ptr) {
    data->dealloc_size += sizeof(T);
    delete[] ptr;
  }
};

class MemoryTests {
 public:
  void simpleTest() {
    Allocator<int> allocator;
    {
      cpputils::memory::Memory<int, decltype(allocator)>::init(allocator, 1);
    }
    cpputils::testing::assertEqual(4, allocator.data->alloc_size);
    cpputils::testing::assertEqual(4, allocator.data->dealloc_size);
  }
};

int main() {
  cpputils::testing::test(MemoryTests{});
  return 0;
}