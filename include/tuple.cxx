export module annotest:tuple;

import std;

import :utils;

using std::size_t;

namespace annotest {
template <template <typename...> class Template, typename T>
struct is_instantiation_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_instantiation_of<Template, Template<Args...>> : std::true_type {};

template <template <typename...> class Template, typename T>
struct is_instantiation_of<Template, const T> : is_instantiation_of<Template, T> {};

template <template <typename...> class Template, typename T>
struct is_instantiation_of<Template, T&> : is_instantiation_of<Template, T> {};

template <template <typename...> class Template, typename T>
struct is_instantiation_of<Template, T&&> : is_instantiation_of<Template, T> {};

template <size_t I>
struct member_name {
  // NOTE: This template only handles indices up to 9999. For I >= 10000 the generated names
  // will overflow the character array and produce undefined behavior. This should never be
  // the case. Might want to consider why you're creating a test with 9999+ parameters...
  static constexpr auto internal_value() {
    if constexpr (I < 10) {
      return std::array<char, 3>{'m', static_cast<char>('0' + I), '\0'};
    } else if constexpr (I < 100) {
      return std::array<char, 4>{'m', static_cast<char>('0' + I / 10),
                                 static_cast<char>('0' + I % 10), '\0'};
    } else if constexpr (I < 1000) {
      return std::array<char, 5>{'m', static_cast<char>('0' + I / 100),
                                 static_cast<char>('0' + (I / 10) % 10),
                                 static_cast<char>('0' + I % 10), '\0'};
    } else {
      return std::array<char, 7>{'m',
                                 static_cast<char>('0' + I / 1000),
                                 static_cast<char>('0' + (I / 100) % 10),
                                 static_cast<char>('0' + (I / 10) % 10),
                                 static_cast<char>('0' + I % 10),
                                 '\0'};
    }
  }

  static constexpr auto value = member_name<I>::internal_value();
};

template <typename... Ts, std::size_t... Is>
consteval auto make_specs(std::index_sequence<Is...>) {
  // Explicitly typing the array prevents CTAD failure on empty packs
  return std::array<std::meta::info, sizeof...(Ts)>{
      std::meta::data_member_spec(^^Ts, {
                                            .name = member_name<Is>::value.data()})...};
}

export template <typename... Ts>
struct tuple {
  struct storage;

  consteval {
    auto storage_meta = ^^storage;

    // Generate the specs using an index sequence to keep 'I' constant
    auto specs = make_specs<Ts...>(std::make_index_sequence<sizeof...(Ts)>{});

    std::meta::define_aggregate(storage_meta, specs);
  }

  static constexpr auto nsdms = std::define_static_array(
      std::meta::nonstatic_data_members_of(^^storage, std::meta::access_context::current()));
  static constexpr auto types_refl =
      std::define_static_array(std::array<std::meta::info, sizeof...(Ts)>{(^^Ts)...});

  storage s;

  constexpr tuple(Ts... ts) : s{std::forward<Ts>(ts)...} {}

  static consteval auto size() -> std::size_t { return sizeof...(Ts); }

  template <std::size_t index>
  constexpr const auto& get() const {
    return s.[:nsdms[index]:];
  }
  template <std::size_t index>
  constexpr auto& get() {
    return s.[:nsdms[index]:];
  }
};

template <typename T>
concept tuple_t = is_instantiation_of<tuple, T>::value;

template <tuple_t... Tuples>
consteval auto get_all_types() {
  std::vector<std::meta::info> all_types;
  all_types.reserve((Tuples::types_refl.size() + ...));

  (all_types.insert(all_types.end(), Tuples::types_refl.begin(), Tuples::types_refl.end()), ...);
  return std::define_static_array(all_types);
}

template <tuple_t... Tuples>
constexpr std::size_t tuple_cat_size_v = (std::remove_reference_t<Tuples>::size() + ...);

template <std::size_t I, tuple_t tuple>
constexpr decltype(auto) get_tuple_element(tuple&& t) {
  return t.template get<I>();
}

// -----------------------------
// Map global index -> (tuple index, local index)
// -----------------------------
template <std::size_t I, tuple_t First, tuple_t... Rest>
constexpr decltype(auto) tuple_cat_get(First&& first, Rest&&... rest) {
  constexpr std::size_t first_size = std::remove_reference_t<First>::size();

  if constexpr (I < first_size) {
    return std::forward<First>(first).template get<I>();
  } else {
    return tuple_cat_get<I - first_size>(std::forward<Rest>(rest)...);
  }
}

// Base case: single tuple
template <std::size_t I, tuple_t Last>
constexpr decltype(auto) tuple_cat_get(Last&& last) {
  return last.template get<I>();
}

template <tuple_t... Tuples, std::size_t... I>
constexpr auto tuple_cat_impl(std::index_sequence<I...>, Tuples&&... tuples) {
  static constexpr auto all_types = get_all_types<std::remove_reference_t<Tuples>...>();
  using Result = typename[:std::meta::substitute(^^tuple, all_types):];

  return Result{tuple_cat_get<I>(std::forward<Tuples>(tuples)...)...};
}

export template <tuple_t... Tuples>
constexpr auto tuple_cat(Tuples&&... tuples) {
  constexpr std::size_t N = tuple_cat_size_v<Tuples...>;
  return tuple_cat_impl(std::make_index_sequence<N>{}, std::forward<Tuples>(tuples)...);
}
}  // namespace annotest