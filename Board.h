#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cstdint>

enum Stone : uint8_t { Empty = 0, White = 1, Black = 2 };

class Board;
class GroupManager;

struct Group {
	int id;
	std::set<int> stones;
	Stone color;

	Group(int id, const std::set<int>& stones, Stone color);
	Group();
	std::set<int> liberties(const Board& board) const;
	std::set<int> antiLiberties(const Board& board) const;
	std::set<int> friends(const Board& board) const;
};

class GroupManager {
private:
	std::map<int, Group> groups;
	int counter;
	void mergeGroup(int a, int b);

public:
	GroupManager();
	GroupManager(const GroupManager& manager);
	void addStone(Stone s, int ind, Board& board);
	void removeGroup(int id);
	Stone getColor(int id) const;
	const Group& getGroup(int id) const;
	int find(int a) const;
	std::set<int> getIsolated(const Board& board) const;
	
	const std::map<int, Group> getMap() const; // TEST ONLY

};

class Board {
private:
	int size;
	std::vector<Stone> board;
	std::vector<Stone> prev; // more efficient ways for ko rule, but this is easy
	GroupManager manager;

	int ind(int row, int col) const;
	void removeIsolated();
	void removeGroupMembers(int id);

public:
	Board(int s);
	Board(const Board& other);
	Board& operator=(const Board& other);
	void setColor(int row, int col, Stone color);
	bool hasStone(int ind) const;
	Stone getStone(int ind) const;
	int getSize() const;
	const std::map<int, Group> getGroups() const; // test method
	std::set<int> getNeighbors(int ind) const;
	bool isConnected(Stone color) const;
	bool violatesKo(int row, int col, Stone color) const;
	void printBoard() const;
	void prettyPrintBoard() const;
	void printState() const;

	// our evaluation functions

	int evaluate() const;
	int victoryStatus() const;
	int numLiberties(Stone color) const;
	int numAdjacent(Stone color) const;
	int largestGroup(Stone color) const;
	int pieceDiff() const;
	int centerControl() const;
};
