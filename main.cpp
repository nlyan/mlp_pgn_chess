#include <mlp/chess/board.hpp>
#include <mlp/chess/pgn_parser.hpp>

#include <filesystem>
#include <iostream>

using namespace mlp;

static void
print_usage(std::ostream& os, const char* const message = nullptr)
{
    static auto const exe = std::filesystem::read_symlink("/proc/self/exe").filename().string();
    if (message)
    {
        os << message << "\n";
    }
    os << "Usage: " << exe << " <game.pgn>\n";
}

int main(int const argc, char** const argv)
try
{
    std::ios::sync_with_stdio(false);
    if (argc < 2)
    {
        print_usage(std::cout, "Missing pgn file path");
        return EXIT_FAILURE;
    }

    chess::pgn::parser pgn_parser;
    chess::board chess_board;

    std::vector<chess::pgn::player_move> moves;
    pgn_parser.parse_file(argv[1], moves);

    std::cout << "Begin!:\n"
              << chess_board << "\n";

    int move_id = 2; // A "move" in chess is two player moves
    for (auto& move_var: moves)
    {
        if (!std::holds_alternative<chess::pgn::standard_move>(move_var))
        {
            continue;
        }
        auto& move = std::get<chess::pgn::standard_move>(move_var);
        if (!chess_board.identify_moving_piece(move.colour, move.piece,
                                               move.from,move.to, move.capture))
        {
            std::cout << "Failed to identify piece for move " << (move_id/2) << ": " << move << "\n";
            return EXIT_FAILURE;
        }

        std::cout << "\nMove " << (move_id/2) << ": " << move << "\n";
        chess_board.move(move.from, move.to, move.capture);
        std::cout << chess_board;
        ++move_id;
    }

    return EXIT_SUCCESS;
}
catch (...)
{
    std::cout.flush();
    throw;
}
