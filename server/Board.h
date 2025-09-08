#pragma once

#include "crow.h"

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cstdint>

enum Stone : uint8_t
{
	Empty = 0,
	White = 1,
	Black = 2
};

class Board;
class GroupManager;

struct Group
{
	int id;
	std::set<int> stones;
	Stone color;

	Group(int id, const std::set<int> &stones, Stone color);
	Group();
	std::set<int> liberties(const Board &board) const;
	std::set<int> antiLiberties(const Board &board) const;
	std::set<int> friends(const Board &board) const;
};

class GroupManager
{
private:
	std::map<int, Group> groups;
	int counter;
	void mergeGroup(int a, int b);

public:
	GroupManager();
	GroupManager(const GroupManager &manager);
	void addStone(Stone s, int ind, Board &board);
	void removeGroup(int id);
	Stone getColor(int id) const;
	const Group &getGroup(int id) const;
	int find(int a) const;
	std::set<int> getIsolated(const Board &board) const;

	const std::map<int, Group> getMap() const; // TEST ONLY
};

class Board
{
private:
	int size; // board is size x size, fyi. not total num of cels. squarical board
	int counter;
	std::vector<Stone> board;
	std::vector<Stone> prev; // more efficient ways for ko rule, but this is easy
	GroupManager manager;

	int ind(int row, int col) const;
	void removeIsolated();
	void removeIsolated(Stone color);
	void removeGroupMembers(int id);

	// private: search helper
	int bestMove(Stone color, int depth) const; // returns SCORE

public:
	Board(int s);
	Board(const Board &other);
	Board &operator=(const Board &other);
	void setColor(int row, int col, Stone color);
	bool hasStone(int ind) const;
	Stone getStone(int ind) const;
	int getSize() const;
	const std::map<int, Group> getGroups() const; // test method
	std::set<int> getNeighbors(int ind) const;
	bool isConnected(Stone color) const;
	bool violatesKo(int row, int col, Stone color) const;
	bool isLegal(int row, int col, Stone color) const;

	// test functions

	void printBoard() const;
	void prettyPrintBoard() const;
	void printState() const;
	void play();

	// our evaluation functions

	int evaluate() const;
	int evaluateFor(Stone color) const;
	int victoryStatus() const;
	int numLiberties(Stone color) const;
	int numAdjacent(Stone color) const;
	int largestGroup(Stone color) const;
	int pieceDiff() const;
	int centerControl() const;

	// our search

	int bestMove(Stone color) const; // returns MOVE IND

	// to json

	crow::json::wvalue boardJson() const; // returns board as json

	// help from json

	void setBoard(crow::json::rvalue board);
	// void setColor(crow::json::rvalue ind, crow::json::rvalue color);
	crow::json::wvalue bestMoveJson(Stone player) const;
	crow::json::wvalue isConnectedJson() const;
};
