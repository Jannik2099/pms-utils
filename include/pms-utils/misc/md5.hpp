#include <array>
#include <boost/algorithm/hex.hpp>
#include <string>
#include <string_view>

namespace [[gnu::visibility("default")]] pms_utils {

class MD5 {
public:
    MD5(std::string_view string) : _bytes({}) {
        boost::algorithm::unhex(string.begin(), string.end(), _bytes.data());
    }

    MD5(std::array<char, 16> bytes) : _bytes(bytes){};

    [[nodiscard]] std::string string() const {
        std::string hash;
        boost::algorithm::hex(std::begin(_bytes), std::end(_bytes), std::back_inserter(hash));
        return hash;
    }

    [[nodiscard]] constexpr std::array<char, 16> bytes() const noexcept { return _bytes; };

private:
    std::array<char, 16> _bytes;
};

} // namespace pms_utils
