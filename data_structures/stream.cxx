export module cpputils.data_structures:stream;

import std;

export namespace cpputils::data_structures {

template <typename Fn, typename Child>
struct Filter;

template <typename T>
concept FilterFn = requires(T val) {
  { val(std::declval<T&&>()) } -> std::convertible_to<bool>;
};

template <typename Fn, typename Child>
struct Map;

template <typename Fn, typename Child = int>
struct StreamStart;

template <typename Current, typename NewChild,
          typename Child = typename[:std::meta::template_arguments_of(^^Current)[1]:]>
struct NewType {
  using Type = typename[:std::meta::template_of(^^Current):]<
      typename[:std::meta::template_arguments_of(
                    ^^Current)[0]:],                                          // Fn
                                   typename NewType<Child, NewChild>::Type>;  // Recurse
};

template <typename Current, typename NewChild>
struct NewType<Current, NewChild, int> {
  using Type = typename[:std::meta::template_of(^^Current):]<
      typename[:std::meta::template_arguments_of(^^Current)[0]:],  // Fn
                                                                NewChild>;
};

template <typename Fn, typename Child>
struct Filter {
  using T =
      std::decay_t<typename[:std::meta::type_of(std::meta::parameters_of(^^Fn::operator())[0]):]>;

  Fn filter_fn;
  Child child;

  inline void operator()(T&& val) {
    if (filter_fn(std::forward<T>(val))) {
      child(std::forward<T>(val));
    }
  }

  void reserve(int capacity) { child.reserve(capacity); }

  template <typename FilterType, typename... ForwardArgs>
  Filter of(FilterType&& old, ForwardArgs&&... args) {
    if constexpr (std::is_same_v<Child, int>) {
      return Filter{std::forward<ForwardArgs>(args)...};
    } else {
      return Filter{std::move(old.filter_fn),
                    Child{}.of(std::move(old.child), std::forward<ForwardArgs>(args)...)};
    }
  }
};

template <typename Fn, typename Child>
struct Map {
  using T =
      std::decay_t<typename[:std::meta::type_of(std::meta::parameters_of(^^Fn::operator())[0]):]>;
  using U = std::decay_t<typename[:std::meta::return_type_of(^^Fn::operator()):]>;

  Fn map_fn;
  Child child;

  inline void operator()(T&& val) { child(std::forward<U>(map_fn(std::forward<T>(val)))); }

  void reserve(int capacity) { child.reserve(capacity); }

  template <typename MapType, typename... ForwardArgs>
  Map of(MapType&& old, ForwardArgs&&... args) {
    if constexpr (std::is_same_v<Child, int>) {
      return Map{std::forward<ForwardArgs>(args)...};
    } else {
      return Map{std::move(old.map_fn),
                 Child{}.of(std::move(old.child), std::forward<ForwardArgs>(args)...)};
    }
  }
};

template <typename Fn, typename Child>
struct StreamStart {
  static_assert("StreamStart Fn must take an int parameter");
  using T = std::decay_t<typename[:std::meta::return_type_of(^^Fn::operator()):]>;

  Fn get;
  T* const data;
  int size;
  Child child;
  int idx = 0;

  inline void operator()() {
    if (size != std::numeric_limits<int>::max()) {
      child.reserve(size);
    }
    while (idx < size) {
      child(std::forward<T>(get(data, idx++)));
    }
  }

  template <typename StreamStartOld, typename... ForwardArgs>
  StreamStart of(StreamStartOld&& old, ForwardArgs&&... args) {
    if constexpr (std::is_same_v<Child, int>) {
      return StreamStart{std::forward<ForwardArgs>(args)...};
    } else {
      return StreamStart{std::move(old.get), std::move(old.data), std::move(old.size),
                         Child{}.of(std::move(old.child), std::forward<ForwardArgs>(args)...)};
    }
  }
  template <typename PredicateFn>
  typename NewType<StreamStart<Fn, Child>, Filter<std::decay_t<PredicateFn>, int>>::Type filter(
      PredicateFn filter_fn) {
    return
        typename NewType<StreamStart<Fn, Child>, Filter<std::decay_t<PredicateFn>, int>>::Type{}.of(
            *this, std::move(filter_fn));
  }

  template <typename MapFn>
  typename NewType<StreamStart<Fn, Child>, Map<std::decay_t<MapFn>, int>>::Type map(MapFn map_fn) {
    return typename NewType<StreamStart<Fn, Child>, Map<std::decay_t<MapFn>, int>>::Type{}.of(
        *this, std::move(map_fn));
  }

  template <typename CollectStruct>
  typename NewType<StreamStart<Fn, Child>, CollectStruct>::Type collect(
      CollectStruct collect_struct) {
    return typename NewType<StreamStart<Fn, Child>, CollectStruct>::Type{}.of(
        *this, std::move(collect_struct));
  }
};

}  // namespace cpputils::data_structures