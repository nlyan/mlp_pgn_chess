#include <mlp/chess/square.hpp>

#include <ostream>

namespace mlp::chess
{

std::ostream&
operator<<(std::ostream& os, square const& sq)
{
    os.put(sq.file);
    os.put(sq.rank);
    return os;
}

} // namespace mlp::chess
