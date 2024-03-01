#pragma once

namespace mlp::chess
{

enum class piece_colour: char
{
    Black = 'b',
    White = 'w',
    Null  = ' ',
};

enum class piece_type: char
{
    Bishop = 'B',
    King  = 'K',
    Knight = 'N',
    Pawn   = 'P',
    Queen = 'Q',
    Rook   = 'R',
    Null  = ' '
};

class piece
{
public:
    consteval piece(char const (&init)[3]) noexcept:
        colour_(static_cast<piece_colour>(init[0])),
        type_(static_cast<piece_type>(init[1]))
    {}
    constexpr piece() noexcept = default;
    piece_colour colour() const noexcept { return colour_; };
    piece_type type() const noexcept { return type_; };
    bool is_null() const noexcept { return (colour() == piece_colour::Null) && (type() == piece_type::Null); }
private:
    piece_colour colour_ = piece_colour::Null;
    piece_type type_ = piece_type::Null;
};

} // mlp::chess
