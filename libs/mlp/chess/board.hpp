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
                          char &from_rank, char &from_file,
                          char const to_rank, char const to_file, bool is_capture);

    void move(char from_rank, char from_file, char to_rank, char to_file, bool is_capture);

    bool straight_path_clear_between(chess::square const& src,
                                     chess::square const& dest) const;

    bool diagonal_path_clear_between(chess::square const& src,
                                     chess::square const& dest) const;

private:
    bool is_valid_move(const piece_type type, int from_rank, int from_file,
                       int to_rank, int to_file, bool is_capture);

    bool is_valid_pawn_move(int from_rank, int from_file,
                            int to_rank, int to_file, bool is_capture);

private:
    rank_array ranks_; // Ranks are ordered from whites perspective
};

std::ostream& operator<<(std::ostream& os, board const& board);

} // mlp::chess
