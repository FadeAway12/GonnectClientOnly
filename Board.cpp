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

//group methods

Group::Group(int id, const std::set<int>& stones, Stone color) {
	this->id = id;
	this->stones = stones;
	this->color = color;
}

Group::Group() : id(-1), stones(), color(Stone::Empty) {}

std::set<int> Group::liberties(const Board& board) const { // unoccupied frontiers of group
	std::set<int> liberties;
	for (int ind : stones) {
		std::set<int> neighbors = board.getNeighbors(ind);
		for (int n : neighbors) {
			if (!board.hasStone(n)) {
				liberties.insert(n);
			}
		}
	}
	return liberties;
}

std::set<int> Group::antiLiberties(const Board& board) const { // occupied frontiers
	std::set<int> antiLiberties;
	for (int ind : stones) {
		std::set<int> neighbors = board.getNeighbors(ind);
		for (int n : neighbors) {
			if (board.hasStone(n)) {
				antiLiberties.insert(n);
			}
		}
	}
	return antiLiberties;
}

std::set<int> Group::friends(const Board& board) const { // same color frontiers of group (ie its gotta merge)
	std::set<int> friends;
	for (int ind : stones) {
		std::set<int> neighbors = board.getNeighbors(ind);
		for (int n : neighbors) {
			if (board.hasStone(n) && board.getStone(n) == color) {
				friends.insert(n);
			}
		}
	}
	return friends;
}

//Group manager priv:

void GroupManager::mergeGroup(int a, int b) { // unions two groups by their id, using lowest id for new group


	if (a == b) {
		std::cerr << "can't merge with self";
		return;
	}

	auto pair1 = groups.find(a);
	auto pair2 = groups.find(b);

	if (pair1 == groups.end() || pair2 == groups.end()) {
		std::cerr << "cannot find group.";
		return;
	}


	Group& first = pair1->second;
	Group& second = pair2->second;

	if (first.color != second.color) {
		std::cerr << "can't merge diff colors";
		return;
	}

	std::set<int> set1 = first.stones;
	std::set<int> set2 = second.stones;
	std::set<int> result = set1;

	result.insert(set2.begin(), set2.end());

	Group comb = Group(std::min(a, b), result, first.color);

	groups.erase(std::max(a, b));

	groups[std::min(a, b)] = comb;

}

// Group Manager pub:

GroupManager::GroupManager() {
	counter = 0;
}

GroupManager::GroupManager(const GroupManager& manager) {
	counter = manager.counter;
	groups = manager.groups;
}

void GroupManager::addStone(Stone s, int ind, Board& board) {
	std::set<int> mySet;
	mySet.insert(ind);
	Group init = Group(counter, mySet, s);
	groups[counter] = init;
	std::set<int> friends = init.friends(board);

	int curID = counter;
	for (int taken : friends) {
		if (curID == -1 || find(taken) == -1) {
			std::cerr << "one group doesn't exist";
			return;
		}
		mergeGroup(curID, find(taken));
		curID = find(ind);
	}
	counter++;
}

void GroupManager::removeGroup(int id) {
	groups.erase(id);
}

Stone GroupManager::getColor(int id) const {
	return groups.at(id).color;
}

const Group& GroupManager::getGroup(int id) const { // get group from id
	return groups.at(id);
}

int GroupManager::find(int a) const { // whats the group id of stone "a" 
	for (const auto& pair : groups) {
		const Group& group = pair.second;
		if (group.stones.find(a) != group.stones.end()) return group.id;
	}
	return -1;
}

std::set<int> GroupManager::getIsolated(const Board& board) const { // returns the ID of the isolated groups (no liberties);
	std::set<int> mySet;
	for (const auto& pair : groups) {
		const Group& group = pair.second;
		if (group.liberties(board).size() == 0) {
			mySet.insert(group.id);
		}
	}
	return mySet;
}

const std::map<int, Group> GroupManager::getMap() const {
	return groups;
}

//board methods

//priv:

int Board::ind(int row, int col) const {
	return row * size + col;
}

void Board::removeIsolated() {
	std::set<int> groupIDs = manager.getIsolated(*this);
	for (int id : groupIDs) {
		removeGroupMembers(id);
		manager.removeGroup(id);
	}
}

void Board::removeGroupMembers(int id) {	// helper for removeIsolated
	const Group& g = manager.getGroup(id);
	for (int i : g.stones) {
		board[i] = Stone::Empty;
	}
}
//pub:
Board::Board(int s) :
	size(s), board(s* s, Stone::Empty), prev(s*s, Stone::Empty) {}

Board::Board(const Board& other) {
	size = other.size;
	board = other.board;
	manager = other.manager;
	prev = other.prev;
}

Board& Board::operator=(const Board& other) { // copy assignment
	if (this != &other) {
		size = other.size;
		board = other.board;
		manager = other.manager;
		prev = other.prev;
	}
	return *this;
}

void Board::setColor(int row, int col, Stone color) { // note: in gonnect, suicide is legal. NOTE: DOES NOT CHECK KO! MUST USE violatesKo SEPARATELY!!!!!!!
	prev = board;
	int i{ ind(row, col) };
	if (hasStone(i)) {
		std::cerr << "occupied";
		return;
	}
	board[i] = color;
	manager.addStone(color, i, *this);
	removeIsolated();
}

bool Board::hasStone(int ind) const {
	return board[ind] != Stone::Empty;
}

Stone Board::getStone(int ind) const {
	return board[ind];
}

int Board::getSize() const {
	return size;
}


std::set<int> Board::getNeighbors(int ind) const { //returns a set of the indices of all orthogonally adjacent points
	std::set<int> neighbors;
	if (ind / size > 0) neighbors.insert(ind - size);
	if (ind / size < size - 1) neighbors.insert(ind + size);
	if (ind % size != 0) neighbors.insert(ind - 1);
	if ((ind + 1) % size != 0) neighbors.insert(ind + 1);
	return neighbors;
}

const std::map<int, Group> Board::getGroups() const {
	return manager.getMap();
}

bool Board::isConnected(Stone color) const { // BFS algo to check if a certain color has a connection on the board
	//top to bottom
	std::queue<int> startIndices{};
	std::set<int> visited{};
	for (int i = 0; i < size; i++) {
		if (getStone(i) == color) {
			startIndices.push(i);
			visited.insert(i);
		}
	}
	while (!startIndices.empty()) {
		int ind = startIndices.front();
		
		if (ind / size == size - 1) return true;
		startIndices.pop();
		

		std::set<int> neighbors = getNeighbors(ind);
		for (int n : neighbors) {
			if (visited.find(n) == visited.end() && getStone(n) == color) {
				startIndices.push(n);
				visited.insert(n);
			}
		}

	}
	// left to right
	startIndices = std::queue<int>{};
	visited.clear();

	for (int i = 0; i < size*size; i+=size) {
		if (getStone(i) == color) {
			startIndices.push(i);
			visited.insert(i);
		}
	}
	while (!startIndices.empty()) {
		int ind = startIndices.front();

		if (ind % size == size - 1) return true;
		startIndices.pop();


		std::set<int> neighbors = getNeighbors(ind);
		for (int n : neighbors) {
			if (visited.find(n) == visited.end() && getStone(n) == color) {
				startIndices.push(n);
				visited.insert(n);
			}
		}

	}

	return false;
}

bool Board::violatesKo(int row, int col, Stone color) const {
	Board newBoard = Board(*this);
	newBoard.setColor(row, col, color);
	return newBoard.board == prev;
}

void Board::printBoard() const {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			int idx{ ind(i, j) };
			if (board[idx] == Stone::White) std::cout << 'W';
			else if (board[idx] == Stone::Black) std::cout << 'B';
			else std::cout << ' ';
		}
		std::cout << '\n';
	}
}

void Board::prettyPrintBoard() const {
	std::cout << "   ";
	for (int col = 0; col < size; ++col) {
		std::cout << col%10 << " ";
	}
	std::cout << "\n";

	for (int row = 0; row < size; ++row) {
		std::cout << row%10 << " |";
		for (int col = 0; col < size; ++col) {
			int idx = ind(row, col);
			char stoneChar = ' ';
			if (board[idx] == Stone::White) stoneChar = 'W';
			else if (board[idx] == Stone::Black) stoneChar = 'B';

			std::cout << stoneChar << "|";
		}
		std::cout << "\n";
	}

}

void Board::printState() const {
	std::cout << "CURRENT STATE: \n";
	prettyPrintBoard();
	std::map<int, Group> groups = getGroups();
	std::cout << "GROUP COUNT: " << groups.size() << "\n";
	for (const auto& pair : groups) {
		const Group& group = pair.second;
		std::cout << (group.color == Stone::White ? "W\n" : "");
		std::cout << (group.color == Stone::Black ? "B\n" : "");
		std::cout << "Root Number is " << group.id << "\n";
	}
	std::cout << "\n";
}

int main() {
	std::cout << "hi" << '\n';
	std::map<int, Group> groups;
	Board b{ 9 };
	b.setColor(1, 1, Stone::Black);
	b.setColor(2, 2, Stone::Black);
	b.setColor(3, 1, Stone::Black);
	b.setColor(2, 0, Stone::Black);
	b.printState();
	b.setColor(2, 1, Stone::Black);
	b.printState();

	return 0;
}
