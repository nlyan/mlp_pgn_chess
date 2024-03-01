#pragma once

#include <mlp/chess/pgn_playermove.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace mlp::chess::pgn
{

class parser
{
public:
    parser() noexcept;

    void parse_file(std::filesystem::path const& file_path,
                    std::vector<pgn::player_move>& moves);

    static bool parse_pgn_move(char const*& begin, char const* end,
                               unsigned& move_id,
                               pgn::player_move& white_move,
                               pgn::player_move& black_move);
    void reset();

private:
    std::string move_text_;
};

} // namespace mlp::chess::pgn
