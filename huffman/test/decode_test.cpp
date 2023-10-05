#include "huffman/huffman.hpp"

#include <boost/ut.hpp>

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>

auto main() -> int
{
  using ::boost::ut::expect;
  using ::boost::ut::test;

  namespace huffman = ::starflate::huffman;
  using namespace huffman::literals;

  test("basic") = [] {
    constexpr auto encoded_bytes = std::array{
        std::byte{0b0111'1101},
        std::byte{0b1000'0011},
        std::byte{0b1111'1010},
        std::byte{0b1110'1100},
        std::byte{0b1001'0110},
        std::byte{0b1101'1110}};

    constexpr auto eot = char{'\4'};
    static constexpr auto code_table =  // clang-format off
      huffman::table{
        huffman::table_contents,
        {std::pair{0_c,     'e'},
                  {10_c,    'i'},
                  {110_c,   'n'},
                  {1110_c,  'q'},
                  {11110_c, eot},
                  {11111_c, 'x'}
        }
      };  // clang-format on

    constexpr auto expected = std::array{
        'e', 'x', 'e', 'n', 'e', 'e', 'e', 'e', 'x', 'n',
        'i', 'q', 'n', 'e', 'i', 'e', 'i', 'n', 'n', eot,
    };
    constexpr auto output_buf = [&] {
      auto output_buf = std::array<char, expected.size()>{};

      const auto result = decode(code_table, encoded_bytes, output_buf.begin());

      // result should point to the back of output_buf.
      if (output_buf.end() != result) {
        throw std::runtime_error("assertion failed");
      }
      return output_buf;
    }();

    static_assert(output_buf == expected);
  };
}
