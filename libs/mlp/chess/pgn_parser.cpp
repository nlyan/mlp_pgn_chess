#include <mlp/chess/pgn_parser.hpp>
#include <mlp/chess/utility.hpp>

#include <charconv>
#include <fstream>
#include <iostream>
#include <stack>
#include <variant>

namespace mlp::chess::pgn
{

namespace
{

void
remove_annotations(std::string& str)
{
    std::stack<int> parens;
    int out = 0;
    for (int i = 0; i < str.size(); ++i)
    {
        if (str[i] == '(' || str[i] == '{')
        {
            parens.push(i);
        }
        else if (!parens.empty())
        {
            if (((str[i] == ')') && (str[parens.top()] == '('))
                || ((str[i] == '}') && (str[parens.top()] == '{')))
            {
                parens.pop();
            }
        }
        else
        {
            str[out++] = str[i];
        }
    }
    if (!parens.empty())
    {
        throw std::runtime_error ("PGN movetext ended with open parens (comments/annotations)");
    }
    str.resize(out);
}

bool
skip_one(char const*& ptr, char const* const end, char const c)
{
    if ((ptr != end) && (*ptr == c))
    {
        ++ptr;
        return true;
    }
    return false;
}

void
skip_kleene_star(char const*& ptr, char const* const end, char const c)
{
    while ((ptr != end) && (*ptr == c))
    {
        ++ptr;
    }
}

void skip_ws(char const*& ptr, char const* const end)
{
    return skip_kleene_star(ptr, end, ' ');
}

bool
parse_piece(char const*& ptr, char const* const end, chess::piece_type& piece)
{
    if (ptr == end) {
        return false;
    }
    switch (*ptr)
    {
        case ' ':
            break;
        case 'B':
        case 'K':
        case 'N':
        case 'Q':
        case 'R':
            piece = static_cast<chess::piece_type>(*ptr++);
            return true;
    }
    return false;
}

bool
parse_file(char const*& ptr, char const* const end, char& file)
{
    if (ptr == end) {
        return false;
    }
    switch (*ptr)
    {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
            file = *ptr++;
            return true;
        default:
            return false;
    }
}

bool
parse_rank(char const*& ptr, char const* const end, char& rank)
{
    if (ptr == end) {
        return false;
    }
    if ((*ptr >= '1') && (*ptr <= '8'))
    {
        rank = *ptr++;
        return true;
    }
    return false;
}

bool
parse_square(char const*& begin, char const* const end, char& file, char& rank)
{
    return parse_file(begin, end, file) && parse_rank(begin, end, rank);
}

bool
parse_capture(char const*& begin, char const* const end, bool& capture)
{
    if ((begin != end) && (*begin == 'x'))
    {
        ++begin;
        capture = true;
        return true;
    }
    return false;
}

bool
match_literal(char const*& begin, char const* const end, char const* str)
{
    auto ptr = begin;
    while (*str != '\0')
    {
        if (ptr >= end || *ptr != *str)
        {
            return false;
        }
        ++ptr;
        ++str;
    }
    begin = ptr;
    return true;
}

bool
parse_kingside_castling(char const*& begin, char const* const end, kingside_castling& kc)
{
    return match_literal(begin, end, "O-O");
}

bool
parse_queenside_castling(char const*& begin, char const* const end, queenside_castling& qc)
{
    return match_literal(begin, end, "O-O-O");
}

bool
parse_pawn_promotion(char const*& begin, char const* const end, pgn::standard_move& move)
{
    auto ptr = begin;
    if ((ptr == end) || !skip_one(ptr, end, '='))
    {
        return false; // Backtrack
    }
    chess::piece_type piece = chess::piece_type::Pawn;
    if ((ptr == end) || !parse_piece(ptr, end, piece)
        || (piece == chess::piece_type::Pawn)) // Can't promote Pawn to Pawn (whitespace is detected as Pawn)
    {
        return false; // Backtrack
    }
    move.promoted_to = piece;
    begin = ptr; // Commit
    return true;
}

bool
parse_check_or_mate(char const*& begin, char const* const end, pgn::standard_move& move)
{
    if (begin == end)
    {
        return false;
    }
    switch (*begin)
    {
        case '+':
            move.check = true;
            ++begin;
            return true;
        case '#':
            move.mate = true;
            ++begin;
            return true;
    }
    return false;
}

bool
parse_standard_move(char const*& begin, char const* const end, pgn::standard_move& move)
{
    auto ptr = begin;
    move.piece = chess::piece_type::Pawn;
    if (!parse_piece(ptr, end, move.piece))
    {
        move.piece = piece_type::Pawn;
    }
    /*
     * When disambiguating a player move, PGN uses the following rules:
     *
     * If two (or more) identical pieces can player move to the same square, PGN first tries to disambiguate using the
     * file (column) of departure. If they share the same file but are on different ranks (rows), then the rank of
     * departure is used to disambiguate.
     *
     * If the pieces are on the same file and rank (which would typically only occur in the case of pawns promoting
     * and then one of the promoted pieces returning to the back rank, a very rare occurrence), then the piece is
     * identified by _both_ file and rank of departure.
     */
    move.from.file = 0; // So we can detect failure
    move.from.rank = 0;
    parse_file(ptr, end, move.from.file); // Optional, so ignore failure
    parse_rank(ptr, end, move.from.rank); // Optional, so ignore failure
    move.capture = false;
    parse_capture(ptr, end, move.capture); // Optional, so ignore failure
    move.to.file = 0; // So we can detect failure
    move.to.rank = 0;
    if (!parse_square(ptr, end, move.to.file, move.to.rank))
    {
        // If we just saw a capture 'x' then a destination square should have followed
        if (move.capture)
        {
            move = pgn::standard_move{};
            return false;
        }
        // If we eagerly parsed the destination square as the departing square, then
        // both file and rank must be present.
        if (!move.from.file || !move.from.rank)
        {
            move = pgn::standard_move{};
            return false;
        }
        std::swap(move.to.file, move.from.file);
        std::swap(move.to.rank, move.from.rank);
    }
    move.promoted_to = chess::piece_type::Null;
    parse_pawn_promotion(ptr, end, move);
    move.check = false;
    move.mate = false;
    parse_check_or_mate(ptr, end, move);
    begin = ptr; // Commit
    return true;
}

bool
parse_player_move(char const*& begin, char const* const end, pgn::player_move& move)
{
    auto ptr = begin;
    if (parse_queenside_castling(ptr, end, move.emplace<pgn::queenside_castling>()))
    {
        move = queenside_castling{};
        begin = ptr;
        return true;
    }
    else if (parse_kingside_castling(ptr, end, move.emplace<pgn::kingside_castling>()))
    {
        move = kingside_castling{};
        begin = ptr;
        return true;
    }
    else if (parse_standard_move(ptr, end, move.emplace<pgn::standard_move>()))
    {
        begin = ptr;
        return true;
    }
    else
    {
        move = std::monostate{};
        return false;
    }
}

bool
parse_game_result(char const*& begin, char const* const end)
{
    return match_literal(begin, end, "1-0")
           || match_literal(begin, end, "0-1")
           || match_literal(begin, end, "1/2-1/2")
           || match_literal(begin, end, "*");
}

} //anonymous namespace

parser::parser() noexcept
{
}

void
parser::parse_file(std::filesystem::path const& file_path,
                   std::vector<pgn::player_move>& moves)
{
    if (!exists(file_path)) {
        throw std::runtime_error("Could not open PGN file: " + absolute(file_path).string());
    }

    std::ifstream ifs;
    ifs.exceptions(std::ios::badbit);
    ifs.open(file_path.string());
    reset();

    {
        std::string line;
        while (std::getline(ifs, line))
        {
            // Remove tag lines
            if (line.empty() || (line[0] == '['))
            {
                continue;
            }
            // Remove end of line comments
            auto const semi_colon_pos = line.rfind(';');
            if (semi_colon_pos != std::string::npos)
            {
                line.resize(semi_colon_pos);
            }
            while (!line.empty() && line.back() == ' ')
            {
                line.pop_back();
            }
            // Aggregate the player_move text
            if (!line.empty())
            {
                if (!move_text_.empty() && (line.front() != ' '))
                    move_text_ += ' ';
                move_text_ += line;
            }
        }
    }

    remove_annotations(move_text_);
    std::cout << move_text_ << std::endl;

    char const* mt_itr = move_text_.data();
    char const* const mt_end = mt_itr + move_text_.size();
    unsigned int move_id = 0;
    pgn::player_move white_move, black_move;
    moves.clear();

    while (parse_pgn_move(mt_itr, mt_end, move_id, white_move, black_move))
    {
        std::cout << "Move " << move_id << ": " << white_move << ", " << black_move << "\n";
        moves.push_back(white_move);
        moves.push_back(black_move);
    }
    if (std::distance(mt_itr, mt_end) > 0)
    {
        throw std::runtime_error("Failed to parse movetext: " + std::string(mt_itr));
    }
    move_text_.clear();
}

bool
parser::parse_pgn_move(char const*& begin, const char *end,
                       unsigned& move_id,
                       pgn::player_move& white_move, pgn::player_move& black_move)
{
    auto ptr = begin;
    skip_ws(ptr, end);
    auto id_conv = std::from_chars(ptr, end, move_id);
    if (id_conv.ec != std::errc{}) {
        return false;
    }
    ptr = id_conv.ptr;
    if (!skip_one(ptr, end, '.')) {
        return false;
    }
    skip_ws(ptr, end);
    if (!parse_player_move(ptr, end, white_move))
    {
        return false;
    }
    std::visit (overloaded([](auto& move) { move.colour = piece_colour::White; },
                                  [](std::monostate&){}), white_move);
    skip_ws(ptr, end);
    if (!parse_player_move(ptr, end, black_move))
    {
        if (parse_game_result(ptr, end))
        {
            begin = ptr;
            return true;
        }
        return false;
    }
    std::visit (overloaded([](auto& move) { move.colour = piece_colour::Black; },
                                  [](std::monostate&){}), black_move);
    begin = ptr;
    return true;
}

void
parser::reset()
{
    move_text_.clear();
}

} // namespace mlp::chess::pgn
