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

int Board::bestMove(Stone color) const
{ // 1 move depth (lol) -> will improve later. returns a MOVE.
	std::tuple<int, int> best{-1, -1000000};
	for (int i = 0; i < size * size; i++)
	{ // iterate over all empty spots
		int row{i / size};
		int col{i % size};
		if (isLegal(row, col, color))
		{
			Board newBoard = Board{*this};
			newBoard.setColor(row, col, color);
			int eval{};

			if (color == Stone::White)
			{
				eval = -newBoard.bestMove(Stone::Black, 1);
			}
			else
			{
				eval = -newBoard.bestMove(Stone::White, 1);
			}
			if (eval > std::get<1>(best))
			{
				std::tuple<int, int> newBest{i, eval};
				best = newBest;
			}
		}
	}
	std::cout << "BEST MOVE: " << std::get<0>(best) / size << " " << std::get<0>(best) % size << "\n"
			  << "EVAL SCORE: " << std::get<1>(best) << "\n";
	return std::get<0>(best);
}

int Board::bestMove(Stone color, int depth) const
{ // 2 move depth (lol) -> will improve later. RETURNS A SCORE, NOT MOVE. HELPER!!!

	if (depth == 0 || isConnected(Stone::White) || isConnected(Stone::Black))
	{
		std::tuple<int, int> best{-1, -1000000};
		for (int i = 0; i < size * size; i++)
		{ // iterate over all empty spots
			int row{i / size};
			int col{i % size};
			if (isLegal(row, col, color))
			{
				Board newBoard = Board{*this};
				newBoard.setColor(row, col, color);
				int eval = newBoard.evaluateFor(color);
				if (eval > std::get<1>(best))
				{
					std::tuple<int, int> newBest{i, eval};
					best = newBest;
				}
			}
		}
		return std::get<1>(best);
	}
	else
	{
		std::tuple<int, int> best{-1, -1000000};
		for (int i = 0; i < size * size; i++)
		{ // iterate over all empty spots
			int row{i / size};
			int col{i % size};
			if (isLegal(row, col, color))
			{
				Board newBoard = Board{*this};
				newBoard.setColor(row, col, color);
				int eval{};
				if (color == Stone::White)
				{
					eval = -newBoard.bestMove(Stone::Black, depth - 1);
				}
				else
				{
					eval = -newBoard.bestMove(Stone::White, depth - 1);
				}
				if (eval > std::get<1>(best))
				{
					std::tuple<int, int> newBest{i, eval};
					best = newBest;
				}
			}
		}
		return std::get<1>(best);
	}

	// std::cout << "BEST MOVE: " << std::get<0>(best) / size << " " << std::get<0>(best) % size << "\n" << "EVAL SCORE: " << std::get<1>(best) << "\n";
	// return 1;
}
