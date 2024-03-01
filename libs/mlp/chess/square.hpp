#pragma once

#include <compare>
#include <iosfwd>

namespace mlp::chess
{

class square
{
friend auto operator<=>(const square&, const square&) = default;
public:
    square() noexcept = default;
    square(char file, char rank) noexcept: file(file), rank(rank) {}
    char file = '\0';
    char rank = '\0';
};

std::ostream& operator<<(std::ostream& os, square const&);

} // namespace mlp::chess
