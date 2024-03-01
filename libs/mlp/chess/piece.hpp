#pragma once

namespace mlp::chess
{

enum class piece_colour: char
{
    Black = 'b',
    White = 'w',
    None  = ' ',
};

enum class piece_type: char
{
    Bishop = 'B',
    King  = 'K',
    Knight = 'N',
    Pawn   = 'P',
    Queen = 'Q',
    Rook   = 'R',
    None  = ' '
};

class piece
{
public:
    constexpr piece() noexcept = default;
    consteval piece(char const (&init)[3]) noexcept:
        colour_(static_cast<piece_colour>(init[0])),
        type_(static_cast<piece_type>(init[1]))
    {}
    piece_colour colour() const noexcept { return colour_; };
    piece_type type() const noexcept { return type_; };
private:
    piece_colour colour_ = piece_colour::None;
    piece_type type_ = piece_type::None;
};

} // mlp::chess
