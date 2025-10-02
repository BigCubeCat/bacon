#include "utils.hpp"

bool isValidIPv4WithPort(const QString& input) {
    static const QRegularExpression re(
        R"(^(?:(localhost)|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})):(6553[0-5]|655[0-2]\d|65[0-4]\d{2}|6[0-4]\d{3}|[1-5]\d{4}|[1-9]\d{0,3})$)",
        QRegularExpression::CaseInsensitiveOption);

    return re.match(input).hasMatch();
}
