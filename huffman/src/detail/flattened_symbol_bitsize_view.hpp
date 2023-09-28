#pragma once

#include <cstddef>
#include <iterator>
#include <numeric>
#include <ranges>

namespace starflate::huffman::detail {

template <std::ranges::view V>
class flattened_symbol_bitsize_view
    : public std::ranges::view_interface<flattened_symbol_bitsize_view<V>>
{
  V base_;
  std::size_t size_;

  using child_view_type = std::remove_cvref_t<std::ranges::range_value_t<V>>;

public:
  using base_type = V;

  class iterator
  {
    const flattened_symbol_bitsize_view* parent_{};
    std::size_t offset1_{};
    std::size_t offset2_{};

  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ranges::range_difference_t<child_view_type>;
    using value_type = std::ranges::range_value_t<child_view_type>;
    using reference = std::ranges::range_reference_t<child_view_type>;
    using pointer = void;

    iterator() = default;

    constexpr iterator(
        const flattened_symbol_bitsize_view& parent,
        std::size_t offset1,
        std::size_t offset2)
        : parent_{&parent}, offset1_{offset1}, offset2_{offset2}
    {}

    [[nodiscard]]
    constexpr auto
    operator*() const -> reference
    {
      using D = difference_type;
      return parent_
          ->base()[static_cast<D>(offset1_)][static_cast<D>(offset2_)];
    }

    constexpr auto operator++() & -> iterator&
    {
      using D = difference_type;

      if (++offset2_ ==
          static_cast<std::size_t>(
              parent_->base()[static_cast<D>(offset1_)].size())) {
        ++offset1_;
        offset2_ = {};
      }

      return *this;
    }

    constexpr auto operator++(int) -> iterator
    {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    friend constexpr auto
    operator==(const iterator&, const iterator&) -> bool = default;
  };

  constexpr explicit flattened_symbol_bitsize_view(V base)
      : base_{std::move(base)},
        size_{std::accumulate(
            std::ranges::cbegin(base_),
            std::ranges::cend(base_),
            0UZ,
            [](auto acc, const auto& subrange) {
              return acc + subrange.size();
            })}
  {}

  [[nodiscard]]
  constexpr auto base() const -> base_type
  {
    return base_;
  }

  [[nodiscard]]
  constexpr auto size() const -> std::size_t
  {
    return size_;
  }

  [[nodiscard]]
  constexpr auto begin() const -> iterator
  {
    return {*this, 0, 0};
  }

  [[nodiscard]]
  constexpr auto end() const -> iterator
  {
    return {*this, base().size(), 0};
  }
};

}  // namespace starflate::huffman::detail

// flattened_symbol_bitsize_view is only created to adapt an input range during
// construction
template <class V>
inline constexpr bool std::ranges::enable_borrowed_range<
    ::starflate::huffman::detail::flattened_symbol_bitsize_view<V>> = true;
