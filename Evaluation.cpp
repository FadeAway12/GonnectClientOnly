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

int Board::evaluate() const {
	return victoryStatus() +
		(numLiberties(Stone::White)-numLiberties(Stone::Black)) * 3 + 
		(numAdjacent(Stone::White)-numAdjacent(Stone::Black)) + 
		(largestGroup(Stone::White)-largestGroup(Stone::Black)) * 2 +
		pieceDiff() * 2 + centerControl();
}

int Board::victoryStatus() const {
	if (isConnected(Stone::Black)) {
		return -100000;
	}
	else if (isConnected(Stone::White)) {
		return 100000;
	}
	return 0;
}

int Board::numLiberties(Stone color) const {
	std::set<int> libs{};
	for (int ind = 0; ind < size * size; ind++) {
		if (getStone(ind) != color) continue;
		for (int n : getNeighbors(ind)) {
			if (!hasStone(n)) libs.insert(n);
		}
	}
	return libs.size();
}

int Board::numAdjacent(Stone color) const { //friendly or empty
	int cnt{};
	for (int ind = 0; ind < size * size; ind++) {
		if (getStone(ind) != color) continue;
		for (int n : getNeighbors(ind)) {
			if (!hasStone(n) || getStone(n) == color) cnt++;
		}
	}
	return cnt;
}

int Board::largestGroup(Stone color) const {
	std::map<int, Group> map = getGroups();
	int sz{};
	for (auto& pair : map) {
		int newSize = pair.second.stones.size();
		if (newSize > sz && pair.second.color == color) sz = newSize;
	}
	return sz;
}

int Board::pieceDiff() const {
	int diff{};
	for (int i = 0; i < board.size(); i++) {
		if (board[i] == Stone::White) diff++;
		else if (board[i] == Stone::Black) diff--;
	}
	return diff;
}

int Board::centerControl() const {
	int diff{};
	for (int row = size/4; row < 3*size/4; row++) {
		for (int col = size / 4; col < 3 * size / 4; col++) {
			int i = ind(row, col);
			if (board[i] == Stone::White) {
				diff += (1.0 / (abs(size / 2.0-row)+abs(size/2.0-col)))*3;
			}
			else if (board[i] == Stone::Black) {
				diff -= (1.0 / (abs(size / 2.0 - row) + abs(size / 2.0 - col)))*3;
			}
		}
	}
	return diff;
}

//center control