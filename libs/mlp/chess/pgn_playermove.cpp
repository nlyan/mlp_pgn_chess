#include <mlp/chess/pgn_playermove.hpp>
#include <mlp/chess/utility.hpp>

#include <ostream>

namespace mlp::chess::pgn
{

std::ostream&
operator<<(std::ostream& os, kingside_castling const& kc)
{
    os << static_cast<char>(kc.colour) << "(Kingside Castling)";
    return os;
}

std::ostream&
operator<<(std::ostream& os, queenside_castling const& qc)
{
    os << static_cast<char>(qc.colour) << "(Queenside Castling)";
    return os;
}

std::ostream&
operator<<(std::ostream& os, standard_move const& move)
{
    os << static_cast<char>(move.colour) << static_cast<char>(move.piece);
    if ((move.src.file != 0) || (move.src.rank != 0))
    {
        os << " at " << ((move.src.file != 0) ? static_cast<char>(move.src.file) : '?')
           << ((move.src.rank != 0) ? static_cast<char>(move.src.rank) : '?');
    }
    os << " to " << move.dest.file << move.dest.rank;
    if (move.is_capture)
    {
        os << " (capture)";
    }
    if (move.is_check)
    {
        os << " (check)";
    }
    else if (move.is_mate)
    {
        os << " (checkmate)";
    }
    return os;
}

std::ostream&
operator<<(std::ostream& os, player_move const& move)
{
    std::visit(overloaded([&os](auto const& actual_move){ os << actual_move; },
                                [&os](std::monostate const&){ os << "(not taken)"; }), move);
    return os;
}

} // namespace pgn::chess::pgn
