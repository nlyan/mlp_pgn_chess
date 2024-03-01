#pragma once

#include <mlp/chess/piece.hpp>
#include <mlp/chess/square.hpp>

#include <array>
#include <iosfwd>

namespace mlp::chess
{

class board
{
public:
    using rank_type = std::array<piece, 8>;
    using rank_array = std::array<rank_type, 8>;

    board() noexcept;
    rank_array const& ranks() const noexcept { return ranks_; }

    bool
    identify_moving_piece(piece_colour colour, piece_type type,
                          chess::square& src, chess::square const& dest,
                          bool is_capture);

    void move(chess::square const& from, chess::square const& to, bool is_capture);

    bool empty_at(chess::square const& square) const noexcept;

    bool straight_path_is_clear_between(chess::square const& src,
                                        chess::square const& dest) const;

    bool diagonal_path_is_clear_between(chess::square const& src,
                                        chess::square const& dest) const;

private:
    bool is_valid_move(const piece_type type, chess::square const& src, chess::square const& dest, bool is_capture);
    bool is_valid_pawn_move(chess::square const& src, chess::square const& dest, bool is_capture);

private:
    rank_array ranks_; // Ranks are ordered from whites perspective
};

std::ostream& operator<<(std::ostream& os, board const& board);

} // mlp::chess
