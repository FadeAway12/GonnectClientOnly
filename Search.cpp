#include <iostream>
#include <sstream>
#include <bitset>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cstdint>
#include <queue>

#include "Board.h"

int Board::bestMove(Stone color) const { // 1 move depth (lol) -> will improve later
	std::tuple<int, int> best{-1, -1000000};
	for (int i = 0; i < size * size; i++) { //iterate over all empty spots
		int row{ i / size };
		int col{ i % size };
		if (!hasStone(i) && !violatesKo(row, col, color)) {
			Board newBoard = Board{ *this };
			newBoard.setColor(row, col, color);
			int eval = evaluateFor(color);
			if (eval > std::get<1>(best)) {
				std::tuple<int, int> newBest{ i, eval };
				best = newBest;
			}
		}
	}
	return std::get<0>(best);
}
