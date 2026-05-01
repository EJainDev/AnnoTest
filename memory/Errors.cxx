export module cpputils.memory:Errors;

namespace cpputils::memory {
enum class Error {
  SystemError,
  AllocationGreaterThanChunkSize,
};
}