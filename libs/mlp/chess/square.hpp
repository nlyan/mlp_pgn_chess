#pragma once

#include <compare>

namespace mlp::chess
{

class square
{
friend auto operator<=>(const square&, const square&) = default;
public:
    square() noexcept = default;
    square(char rank, char file) noexcept: rank(rank), file(file) {}
    char rank = '\0';
    char file = '\0';
};

} // namespace mlp::chess
