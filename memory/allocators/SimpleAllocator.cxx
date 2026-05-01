export module cpputils.memory:SimpleAllocator;

import std;

import :Errors;

namespace cpputils::memory::allocators {
/**
 * @brief A basic C++ allocator that first calls malloc and then initializes each object
 *
 * @tparam T The type of object(s) to allocate
 */
export template <typename T>
struct SimpleAllocator {
  /**
   * @brief Allocates count objects of type T and initializes them to their default values
   *
   * @param count The number of objects to allocate (must be greater than 0)
   * @param constructor_args Optional arguments to forward to the constructor of T for each object
   * @return std::expected<T*, Error> A pointer to the allocated memory or Error::SystemError if
   * allocation fails
   */
  template <typename... Args>
  static std::expected<T*, Error> alloc(int count = 1, Args&&... constructor_args) pre(count > 0) {
    T* ptr = static_cast<T*>(std::malloc(sizeof(T) * count));
    if (ptr != nullptr) {
      return std::unexpected(Error::SystemError);
    }
    for (int i = 0; i < count; ++i) {
      *ptr = T{std::forward<Args>(constructor_args)...};
    }
    return {ptr};
  }

  /**
   * @brief Destroys and deallocates an object allocated using this allocator
   *
   * @param ptr A pointer to the object(s) to deallocate (must not be nullptr)
   */
  static void dealloc(T* ptr) pre(ptr != nullptr) {
    ptr->~T();
    std::free(ptr);
  }
};
}  // namespace cpputils::memory::allocators