
export module cpputils.memory:SimplePool;

import std;

import :Errors;
import :SimpleAllocator;

namespace cpputils::memory {

export template <typename T, typename Allocator>
class Memory;

namespace allocators {
template <typename T, std::size_t ChunkSize>
struct PoolNode {
  T data[ChunkSize];
  PoolNode* next = nullptr;
};

/**
 * @brief Manages a simple, fixed size memory pool for objects of type T.
 *
 * @details This allocator maintains a linked list of memory chunks, where each chunk can hold a
 * fixed number of objects (ChunkSize). When an allocation request is made, it returns a pointer to
 * the next available object in the current chunk. If the current chunk is full, it moves to the
 * next chunk in the list. When deallocating, it simply adds the deallocated object back to the pool
 * for future reuse as the head of the list. The internal memory is managed through a PoolNode
 * struct whose first element is the data. This allows for easy deallocation by simply treating the
 * pointer to the object as a pointer to the PoolNode and adding it back to the head of the list.
 *
 * @tparam T The type of object(s) to allocate
 * @tparam ChunkSize The number of objects to allocate in each chunk (must be greater than 0)
 * @tparam MaxChunks The maximum number of chunks to allocate (must be greater than 0)
 */
export template <typename T, std::size_t ChunkSize = 1024, int MaxChunks = 1024>
  requires((ChunkSize > 0) && (MaxChunks > 0))
class SimplePool {
 public:
  /**
   * @brief Create a new SimplePool instance with the specified chunk size and maximum number of
   * chunks
   *
   * @return std::optional<SimplePool> A SimplePool instance if initialization is successful
   */
  static std::optional<SimplePool> init() {
    SimplePool pool;
    auto head = SimpleAllocator<PoolNode<T, ChunkSize>>::alloc();
    if (!head) {
      return std::nullopt;
    }

    pool.head = *head;
    auto currentNode = *head;

    for (int i = 1; i < MaxChunks; ++i) {
      auto node = SimpleAllocator<PoolNode<T, ChunkSize>>::alloc();
      if (!node) {
        return std::nullopt;
      }
      node->next = head;
      head = *node;
    }

    return {pool};
  }

 private:
  friend class Memory;

  PoolNode<T, ChunkSize>* head = nullptr;

  /**
   * @brief Allocates memory for an object of type T from the pool
   *
   * @param count The number of objects to allocate (must be greater than 0 and less than or equal
   * to ChunkSize)
   * @return std::expected<T*, Error> A pointer to the allocated memory or an error if allocation
   * fails
   */
  std::expected<T*, Error> alloc(int count) pre(head != nullptr) {
    if (count > ChunkSize) {
      return std::unexpected(Error::AllocationGreaterThanChunkSize);
    }

    auto mem = head->data;
    head = head->next;
    return mem;
  }

  /**
   * @brief Deallocates memory for an object of type T back to the pool (must not be nullptr and
   * must be from this pool)
   */
  void dealloc(T* ptr) pre(ptr != nullptr) {
    auto node = reinterpret_cast<PoolNode<T, ChunkSize>*>(ptr);
    node->next = head;
    head = node;
  }
};
}  // namespace allocators
}  // namespace cpputils::memory