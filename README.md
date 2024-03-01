

# MLP PGN CHESS
### For John


#### Parsing

* The PGN file is simply slurped in line by line
* Comments are stripped from the movetext taking in consideration nested parens
* I implemented a simple back-tracking descent parser to parse the PGN movetext. #

#### Compiling
* Tested on GCC 12.3 (not 12.1), sorry.
* build with "cmake -DMLP_CHESS_DEBUG=1" to get more verbose output out of builds.

#### Unimplemented features:
* There's no support for Pawn "En Passant" moves. The capture piece won't be removed from the board i.e. only captures where you land on the captured square work
* Pawn promotion is parsed from PGN but not implemented (just a few lines of code to add, just run out of time)
* Knight movement is not fully validated during piece selection (I don't check all the necessary squares are empty). I'm just assuming situations where two Knights could ambigiously  to the same destination square are incredibly unlikely.
