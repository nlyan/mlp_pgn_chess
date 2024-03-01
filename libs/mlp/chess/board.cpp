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
board::identify_moving_piece(piece_colour colour, piece_type type,
                             chess::square& src, chess::square const& dest,
                             bool is_capture)
{
    bool found = false;
    char found_rank = 0;
    char found_file = 0;
    // Iterate over the board dest find a piece of the specified colour and type
    for (char rank = '1'; rank <= '8'; ++rank)
    {
        if ((src.rank != 0) && (rank != src.rank))
        {
            continue;
        }
        for (char file = 'a'; file <= 'h'; ++file)
        {
            if ((src.file != 0) && (file != src.file))
            {
                continue;
            }
            const auto& piece = ranks_[rank - '1'][file - 'a'];
            if ((piece.colour() != colour) || (piece.type() != type))
            {
                continue;
            }
            // Check if the piece can move to the destination
            if (is_valid_move(type, square{file, rank}, dest, is_capture))
            {
                if (found)
                {
                    std::cout << "Error: Found two possible chess pieces that can make move: "
                              << static_cast<char>(colour) << static_cast<char>(type) << " at ";
                    std::cout.put(found_file);
                    std::cout.put(found_rank);
                    std::cout << " and ";
                    std::cout.put(file);
                    std::cout.put(rank);
                    std::cout << " can both move to " << dest << ". My implementation is probably b0rked" << std::endl;
                }
                found_rank = rank;
                found_file = file;
                found = true;
            }
        }
    }
    if (found)
    {
        src.rank = found_rank;
        src.file = found_file;
    }
    return found;
}

bool
board::empty_at(chess::square const& square) const noexcept
{
    auto& piece = ranks_[square.rank - '1'][square.file - 'a'];
    return piece.is_null();
}

bool
board::straight_path_is_clear_between(chess::square const& src,
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
    int const rank_dir = (rank_diff > 0) - (rank_diff < 0);
    int const file_dir = (file_diff > 0) - (file_diff < 0);
    auto step = src;
    while (true)
    {
        step.rank += rank_dir;
        step.file += file_dir;
        if (step == dest)
        {
            break;
        }
        if (!empty_at(step))
        {
            return false;
        }
    }
    return true;
}

bool
board::diagonal_path_is_clear_between(chess::square const& src,
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
    int const rank_dir = (rank_diff > 0) - (rank_diff < 0);
    int const file_dir = (file_diff > 0) - (file_diff < 0);
    auto step = src;
    while (true)
    {
        step.rank += rank_dir;
        step.file += file_dir;
        if (step == dest)
        {
            break;
        }
        if (!empty_at(step))
        {
            return false;
        }
    }
    return true;
}

bool
board::is_valid_move(piece_type const type, square const& from, square const& to,
                     bool const is_capture)
{
    int const rank_diff = abs(to.rank - from.rank);
    int const file_diff = abs(to.file - from.file);
    bool const is_diagonal_move = (rank_diff == file_diff);
    bool const is_straight_move = ((rank_diff == 0) || (file_diff == 0));
    switch (type)
    {
        case piece_type::Bishop:
        {
            // Bishops move diagonally
            return is_diagonal_move && diagonal_path_is_clear_between(from, to);
        }
        case piece_type::King:
        {
            // There's only ever one King so there's never any ambiguity, we can safely assume the move
            // is always valid.
            return true;
        }
        case piece_type::Knight:
        {
            // A valid Knight move is either two squares in one direction and one square in the other
            // TODO: Not checking if the square jumped is empty right now. Chance of ambiguity seems low
            if (((rank_diff == 2) && (file_diff == 1)) || ((rank_diff == 1) && (file_diff == 2)))
            {
                return true;
            }
        }
        case piece_type::Pawn:
        {
            return is_valid_pawn_move(from, to, is_capture);
        }
        case piece_type::Queen:
        {
            // Queens move either straight along a rank or file, or diagonally
            return ((is_straight_move && straight_path_is_clear_between(from, to))
                    || (is_diagonal_move && diagonal_path_is_clear_between(from, to)));
        }
        case piece_type::Rook:
        {
            // Rooks move either vertically or horizontally
            return (is_straight_move && straight_path_is_clear_between(from, to));
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
board::is_valid_pawn_move(square const& from, square const& to, bool const is_capture)
{
    // Calculate the direction of movement based on the pawns colour.
    // This works because Pawns can only move forwards.
    int const direction = ranks_[from.rank - '1'][from.file - 'a'].colour() == piece_colour::White ? 1 : -1;
    if (is_capture)
    {
        // Capture move must be diagonal and advance one rank in the correct direction
        if (abs(from.file - to.file) == 1 && (to.rank - from.rank) == direction)
        {
            return true; // Assuming diagonal move is valid if it's a capture.
        }
    }
    else
    {
        // Non-capture move, must move in file
        if (from.file == to.file)
        {
            if (to.rank - from.rank == direction)
            {
                // Single square forward
                return true;
            }
            else if ((((from.rank == '2') && (direction == 1)) || ((from.rank == '7') && (direction == -1)))
                    && ((to.rank - from.rank) == (2 * direction)))
            {
                // Double square forward on first move
                return true;
            }
        }
    }
    return false;
}

void board::perform_queenside_castling(piece_colour const side)
{
    switch (side)
    {
        case piece_colour::Black:
        {
            auto& e8 = ranks_[7]['e' - 'a']; // King
            auto& a8 = ranks_[7]['a' - 'a']; // Rook
            auto& c8 = ranks_[7]['c' - 'a'];
            auto& d8 = ranks_[7]['d' - 'a'];
            c8 = e8;
            e8 = piece{};
            d8 = a8;
            a8 = piece{};
            break;
        }
        case piece_colour::White:
        {
            auto& e1 = ranks_[0]['e' - 'a']; // King
            auto& a1 = ranks_[0]['a' - 'a']; // Rook
            auto& c1 = ranks_[0]['c' - 'a'];
            auto& d1 = ranks_[0]['d' - 'a'];
            c1 = e1;
            e1 = piece{};
            d1 = a1;
            a1 = piece{};
            break;
        }
        case piece_colour::None:
             return;
    }
}

void board::perform_kingside_castling(piece_colour const side)
{
    switch (side)
    {
        case piece_colour::Black:
        {
            auto& e8 = ranks_[7]['e' - 'a']; // King
            auto& h8 = ranks_[7]['h' - 'a']; // Rook
            auto& g8 = ranks_[7]['g' - 'a'];
            auto& f8 = ranks_[7]['f' - 'a'];
            g8 = e8;
            e8 = piece{};
            f8 = h8;
            h8 = piece{};
            break;
        }
        case piece_colour::White:
        {
            auto& e1 = ranks_[0]['e' - 'a']; // King
            auto& h1 = ranks_[0]['h' - 'a']; // Rook
            auto& g1 = ranks_[0]['g' - 'a'];
            auto& f1 = ranks_[0]['f' - 'a'];
            g1 = e1;
            e1 = piece{};
            f1 = h1;
            h1 = piece{};
            break;
        }
        case piece_colour::None:
            return;
    }
}

void
board::move(chess::square const& src, chess::square const& dest, bool const is_capture)
{
    auto& from_piece = ranks_[src.rank - '1'][src.file - 'a'];
    auto& to_piece = ranks_[dest.rank - '1'][dest.file - 'a'];
    if (to_piece.is_null())
    {
    }
    else if (!is_capture)
    {
        throw std::runtime_error("Move to occupied square, but no capture was declared");
    }
    to_piece = from_piece;
    from_piece = chess::piece{};
}

std::ostream&
operator<< (std::ostream& os, board const& board)
{
    auto& ranks = board.ranks();
    int rank_num = 8;
    for (auto rank = rbegin(ranks); rank != rend(ranks); ++rank)
    {
        bool first = true;
#ifdef MLP_CHESS_DEBUG
        os << rank_num-- << " ";
#endif
        for (auto& square: *rank)
        {
            os << (first ? "" : "|") << static_cast<char>(square.colour()) << static_cast<char>(square.type());
            first = false;
        }
        os << "\n";
    }
#ifdef MLP_CHESS_DEBUG
    for (auto file = 'a'; file <= 'h'; ++file)
    {
        os << "  " << file;
    }
    os << "\n";
#endif
    return os;
}

} // mlp::chess