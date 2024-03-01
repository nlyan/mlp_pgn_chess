#include <mlp/chess/board.hpp>

#include <ostream>
#include <iostream>

namespace mlp::chess
{

static constinit board::rank_array const starting_board_state
{{
   /* 1 */ {{"wR","wN","wB","wQ","wK","wB","wN","wR"}},
   /* 2 */ {{"wP","wP","wP","wP","wP","wP","wP","wP"}},
   /* 3 */ {{}},
   /* 4 */ {{}},
   /* 5 */ {{}},
   /* 6 */ {{}},
   /* 7 */ {{"bP","bP","bP","bP","bP","bP","bP","bP"}},
   /* 8 */ {{"bR","bN","bB","bQ","bK","bB","bN","bR"}}
          /*  a    b    c    d    e    f    g    h  */
}};

board::board() noexcept: ranks_(starting_board_state)
{
}

bool
board::identify_moving_piece(piece_colour colour, piece_type type, char& from_rank, char& from_file,
                             char const to_rank, char const to_file, bool is_capture)
{
    bool found = false;
    char found_rank = 0;
    char found_file = 0;
    // Iterate over the board to find a piece of the specified colour and type
    for (int rank = 0; rank < 8; ++rank)
    {
        if ((from_rank != 0) && (rank != (from_rank - '1')))
        {
            continue;
        }
        for (int file = 0; file < 8; ++file)
        {
            if ((from_file != 0) && (file != (from_file - 'a')))
            {
                continue;
            }
            const auto& piece = ranks_[rank][file];
            if ((piece.colour() != colour) || (piece.type() != type))
            {
                continue;
            }
            // Check if the piece can move to the destination
            if (is_valid_move(type, rank, file,
                              to_rank - '1', to_file - 'a', is_capture))
            {
                if (found)
                {
                    std::cout << "ERROR: Found two possible chess pieces that can make move: "
                              << static_cast<char>(colour) << static_cast<char>(type) << " at ";
                    std::cout.put(found_file);
                    std::cout.put(found_rank);
                    std::cout << " and ";
                    std::cout.put('a' + file);
                    std::cout.put('1' + rank);
                    std::cout << " can both to " << to_file << to_rank << ". "
                                 " This is probably a hole in my implementation" << std::endl;
                }
                found_rank = '1' + rank;
                found_file = 'a' + file;
                found = true;
            }
        }
    }
    if (found)
    {
        from_rank = found_rank;
        from_file = found_file;
    }
    return found;
}

bool
board::straight_path_clear_between(chess::square const& src,
                                   chess::square const& dest) const
{
    if (src == dest) [[unlikely]]
    {
        return true;
    }
    int const rank_diff = dest.rank - src.rank;
    int const file_diff = dest.file - src.file;
    bool const is_straight_move = ((rank_diff == 0) || (file_diff == 0));
    if (!is_straight_move) [[unlikely]]
    {
        throw std::runtime_error ("Straight path clearance check called on non-straight move");
    }
    return true; // TODO
}

bool
board::diagonal_path_clear_between(chess::square const& src,
                                   chess::square const& dest) const
{
    if (src == dest) [[unlikely]]
    {
        return true;
    }
    int const rank_diff = dest.rank - src.rank;
    int const file_diff = dest.file - src.file;
    bool const is_diagonal_move = (abs(rank_diff) == abs(file_diff));
    if (!is_diagonal_move) [[unlikely]]
    {
        throw std::runtime_error ("Diagonal path clearance check called on non-diagonal move");
    }
    int const rank_dir = rank_diff / abs(rank_diff);
    int const file_dir = file_diff / abs(file_diff);
    return true; // TODO
}

bool
board::is_valid_move(const piece_type type, int const from_rank, int const from_file,
                     int const to_rank, int const to_file, bool const is_capture)
{
    int const rank_diff = abs(to_rank - from_rank);
    int const file_diff = abs(to_file - from_file);
    bool const is_diagonal_move = (rank_diff == file_diff);
    bool const is_straight_move = ((rank_diff == 0) || (file_diff == 0));
    switch (type)
    {
        case piece_type::Bishop:
        {
            // Bishops move diagonally
            return is_diagonal_move;
        }
        case piece_type::King:
        {
            // There's only ever one King so there's never any ambiguity, we can safely just assume the move
            // is correct.
            return true;
        }
        case piece_type::Knight:
        {
            // A valid Knight move is either two squares in one direction and one square in the other
            if (((rank_diff == 2) && (file_diff == 1)) || ((rank_diff == 1) && (file_diff == 2)))
            {
                return true;
            }
        }
        case piece_type::Pawn:
        {
            return is_valid_pawn_move(from_rank, from_file, to_rank, to_file, is_capture);
        }
        case piece_type::Queen:
        {
            // Queens move either straight along a rank or file, or diagonally
            return (is_straight_move || is_diagonal_move);
        }
        case piece_type::Rook:
        {
            // Rooks move either vertically or horizontally
            return is_straight_move;
        }
        case piece_type::None:
        default:
        {
            break;
        }
    }
    return false;
}

bool
board::is_valid_pawn_move(int from_rank, int from_file, int to_rank, int to_file, bool const is_capture)
{
    // Calculate the direction of movement based on the pawns colour.
    // This works because Pawns can only move forwards.
    int const direction = ranks_[from_rank][from_file].colour() == piece_colour::White ? 1 : -1;
    if (is_capture)
    {
        // Capture move must be diagonal and advance one rank in the correct direction
        if (abs(from_file - to_file) == 1 && (to_rank - from_rank) == direction)
        {
            return true; // Assuming diagonal move is valid if it's a capture.
        }
    }
    else
    {
        // Non-capture move, must move in file
        if (from_file == to_file)
        {
            if (to_rank - from_rank == direction)
            {
                // Single square forward
                return true;
            }
            else if ((((from_rank == 1) && (direction == 1)) || ((from_rank == 6) && (direction == -1)))
                    && ((to_rank - from_rank) == (2 * direction)))
            {
                // Double square forward on first move
                return true;
            }
        }
    }
    return false;
}

void
board::move(char from_rank, char from_file, char to_rank, char to_file, bool is_capture)
{
    auto& from_square = ranks_[from_rank - '1'][from_file - 'a'];
    auto& to_square = ranks_[to_rank - '1'][to_file - 'a'];
    if ((to_square.type() == piece_type::None) && (to_square.colour() == piece_colour::None))
    {
    }
    else if (!is_capture)
    {
        throw std::runtime_error("Move to occupied square when move wasn't tagged as a capture");
    }
    to_square = from_square;
    from_square = chess::piece{};
}

std::ostream&
operator<<(std::ostream& os, board const& board)
{
    auto& ranks = board.ranks();
    int rank_num = 8;
    for (auto rank = rbegin(ranks); rank != rend(ranks); ++rank)
    {
        bool first = true;
        os << rank_num-- << " ";
        for (auto& square: *rank)
        {
            os << (first ? "" : "|") << static_cast<char>(square.colour()) << static_cast<char>(square.type());
            first = false;
        }
        os << "\n";
    }
    os << "  ";
    for (auto file = 'a'; file <= 'h'; ++file)
    {
        os << file << "  ";
    }
    os << "\n";
    return os;
}

} // mlp::chess