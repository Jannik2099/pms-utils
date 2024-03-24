#include <boost/outcome.hpp>
#include <ctime>
#include <iterator>
#include <optional>

namespace [[gnu::visibility("default")]] pms_utils {

enum class ParserStatus { Success, Progress, Fail };

template <typename T, typename U> struct ParserResult {
    ParserStatus status;
    std::optional<U> result;
    T consumed;
};

template <typename T, typename Parser>
ParserResult<typename T::difference_type, typename Parser::attribute_type> try_parse(const T &input,
                                                                                     Parser parser) {
    typename Parser::attribute_type parser_value;
    auto iter = input.begin();
    auto ret = parse(iter, input.end(), parser, parser_value);
    auto consumed = std::distance(input.begin(), iter);
    if (!ret) {
        return ParserResult<typename T::difference_type, typename Parser::attribute_type>{
            ParserStatus::Fail, std::nullopt, consumed};
    }
    if (ret && iter != input.end()) {
        return ParserResult<typename T::difference_type, typename Parser::attribute_type>{
            ParserStatus::Progress, parser_value, consumed};
    }
    if (ret && iter == input.end()) {
        return ParserResult<typename T::difference_type, typename Parser::attribute_type>{
            ParserStatus::Success, parser_value, consumed};
    }
    __builtin_unreachable();
}

} // namespace pms_utils
