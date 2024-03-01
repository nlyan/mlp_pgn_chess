#include <mlp/chess/board.hpp>
#include <mlp/chess/pgn_parser.hpp>
#include <mlp/chess/utility.hpp>

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

#ifdef MLP_CHESS_DEBUG
    std::cout << "\nMove 0:\n" << chess_board << "\n";
#endif

    int move_id = 2;
    for (auto& move_var: moves)
    {
#ifdef MLP_CHESS_DEBUG
        std::cout << "\nMove " << (move_id/2) << ": " <<  move_var << "\n";
#endif

        std::visit(chess::overloaded
        (
            [&](chess::pgn::standard_move& move)
            {
                if (!chess_board.identify_moving_piece(move.colour, move.piece,
                                                       move.src, move.dest, move.is_capture))
                {
                    std::ostringstream oss;
                    oss << "Failed dest find piece dest make move " << (move_id / 2) << ": " << move;
                    throw std::runtime_error(oss.str());
                }
#ifdef MLP_CHESS_DEBUG
                std::cout << "Move " << (move_id/2) << ": " << move <<  "\n";
#endif
                chess_board.move(move.src, move.dest, move.is_capture);
            },
            [&](chess::pgn::kingside_castling& move)
            {
                chess_board.perform_kingside_castling(move.colour);
            },
            [&](chess::pgn::queenside_castling& move)
            {
                chess_board.perform_queenside_castling(move.colour);
            },
            [](auto& other) {} // no op
        ), move_var);

#ifdef MLP_CHESS_DEBUG
        std::cout << chess_board;
#endif
        ++move_id;
    }

#ifdef MLP_CHESS_DEBUG
    std::cout << "\nEndgame: \n";
#endif
    std::cout << chess_board;
    return EXIT_SUCCESS;
}
catch (...)
{
    std::cout.flush();
    throw;
}
