#pragma once

#include <mlp/chess/piece.hpp>
#include <iosfwd>
#include <variant>

namespace mlp::chess::pgn
{

class kingside_castling
{
public:
    piece_colour colour = piece_colour::None;
};

class queenside_castling
{
public:
    piece_colour colour = piece_colour::None;
};

class standard_move
{
public:
    piece_colour colour     = piece_colour::None;
    piece_type piece        = piece_type::Pawn;
    piece_type promoted_to  = piece_type::None;
    char from_file          = 0; // 'a'..'h'
    char from_rank          = 0; // 1..8
    char to_file            = 0; // 'a'..'h'
    char to_rank            = 0; // 1..8
    bool capture            = false;
    bool check              = false;
    bool mate               = false;
};

using player_move = std::variant<std::monostate, standard_move, kingside_castling, queenside_castling>;

std::ostream& operator<<(std::ostream& os, standard_move const& move);
std::ostream& operator<<(std::ostream& os, kingside_castling const& kc);
std::ostream& operator<<(std::ostream& os, queenside_castling const& qc);
std::ostream& operator<<(std::ostream& os, player_move const& move);

} // namespace pgn::chess::pgn
