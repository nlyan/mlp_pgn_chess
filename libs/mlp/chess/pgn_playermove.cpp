#include <mlp/chess/pgn_playermove.hpp>
#include <mlp/chess/util.hpp>

#include <ostream>

namespace mlp::chess::pgn
{

std::ostream&
operator<<(std::ostream& os, kingside_castling const& kc)
{
    os << "(kingside castling)";
    return os;
}

std::ostream&
operator<<(std::ostream& os, queenside_castling const& qc)
{
    os << "(queenside castling)";
    return os;
}

std::ostream&
operator<<(std::ostream& os, standard_move const& move)
{
    os << static_cast<char>(move.colour) << static_cast<char>(move.piece);
    if ((move.from_file != 0) || (move.from_rank != 0))
    {
        os << " at " << ((move.from_file != 0) ? static_cast<char>(move.from_file) : '?')
           << ((move.from_rank != 0) ? static_cast<char>(move.from_rank) : '?');
    }
    os << " to " << move.to_file << move.to_rank;
    if (move.capture)
    {
        os << " (capture)";
    }
    if (move.check)
    {
        os << " (check)";
    }
    else if (move.mate)
    {
        os << " (mate)";
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