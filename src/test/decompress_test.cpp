#include "src/decompress.hpp"

#include <boost/ut.hpp>

#include <array>

auto main() -> int
{
  using ::boost::ut::expect;
  using ::boost::ut::test;

  namespace huffman = ::starflate::huffman;
  test("no compression block type") = [] {
    constexpr auto compressed = std::array{
        std::byte{0b10011111},
        std::byte{5},
        std::byte{0},  // len = 5
        ~std::byte{5},
        ~std::byte{0},  // nlen = 5
        std::byte{'h'},
        std::byte{'e'},
        std::byte{'l'},
        std::byte{'l'},
        std::byte{'o'}};

    const auto expected = std::vector{
        std::byte{'h'},
        std::byte{'e'},
        std::byte{'l'},
        std::byte{'l'},
        std::byte{'o'}};

    const auto actual =
        starflate::decompress(std::span{compressed.data(), compressed.size()});
    expect(::boost::ut::fatal(actual.has_value()))
        << "got error code: " << static_cast<std::int32_t>(actual.error());
    const auto& actual_value = actual.value();
    expect(::boost::ut::fatal(actual_value.size() == expected.size()));
    for (std::size_t i = 0; i < actual_value.size(); ++i) {
      expect(actual_value[i] == expected[i])
          << "at index " << i << "got " << static_cast<int>(actual_value[i])
          << " but expected " << static_cast<int>(expected[i]);
    }
  };
};
