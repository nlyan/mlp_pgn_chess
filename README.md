

# MLP PGN CHESS
### For John

#### Time spent
* Approximately 8 hours

#### Parsing

* The PGN file is simply slurped in line by line
* Comments are stripped from the movetext taking in consideration nested parens
* I implemented a simple back-tracking descent parser to parse the PGN movetext.

#### Compiling
* Tested on GCC 12.3 (not 12.1), sorry.
* build with "cmake -DMLP_CHESS_DEBUG=1" to get more verbose output out of builds.

#### Tests
* I didn't really have time to do anything except manual tests and comparing output to Chess.com

#### Unimplemented features:
* There's no support for ["En Passant" moves](https://www.chess.com/terms/en-passant). The capture piece won't be removed from the board i.e. only captures where you land on the captured square work
* Pawn promotion is parsed from PGN but not implemented (just a few lines of code to add, just run out of time)
* Knight movement is not fully validated during piece selection (I don't check all the necessary squares are empty). I'm just assuming situations where two Knights could ambigiously move to the same destination square are rare in your tests...
