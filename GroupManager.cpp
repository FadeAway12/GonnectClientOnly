#include <iostream>
#include <sstream>
#include <bitset>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

#include "Board.h"
#include "GroupManager.h"

using namespace Gonnect;

namespace Gonnect {

	struct Group {
		int id;
		std::set<int> stones;
		Stone color;

		Group(int id, const std::set<int>& stones, Stone color) {
			this->id = id;
			this->stones = stones;
			this->color = color;
		}

		std::set<int> liberties(const Board& board) const { // unoccupied frontiers of group
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

		std::set<int> antiLiberties(const Board& board) const { // same color frontiers of group (ie its gotta merge)
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

		std::set<int> friends(const Board& board) const { // same color frontiers of group (ie its gotta merge)
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
	};

	struct GroupManager {

	private:
		std::map<int, Group> groups;
		int counter;
		void mergeGroup(int a, int b) { // unions two groups by their id, using lowest id for new group

			a = find(a);
			b = find(b);

			if (a == b) {
				std::cerr << "can't merge with self";
				return;
			}

			Group& first = groups[a];
			Group& second = groups[b];

			std::set<int> set1 = first.stones;
			std::set<int> set2 = second.stones;
			std::set<int> result = set1;

			result.insert(set2.begin(), set2.end());

			Group comb = Group(first.id, result, first.color);

			groups.erase(std::max(a, b));

			groups[std::min(a, b)] = comb;

		}

	public:
		GroupManager() {
			counter = 0;
		}
		void addStone(Stone s, int ind, Board& board) {
			std::set<int> mySet;
			mySet.insert(ind);
			Group init = Group(counter, mySet, s);
			groups[counter] = init;
			std::set<int> friends = init.friends(board);

			for (int taken : friends) {
				mergeGroup(counter, taken);
			}
			counter++;
		}

		void removeGroup(int id) {
			groups.erase(id);
		}

		const Group& getGroup(int id) const { // get group from id
			return groups.at(id);
		}

		int find(int a) const { // whats the group id of stone "a" 
			for (const auto& pair : groups) {
				const Group& group = pair.second;
				if (group.stones.find(a) != group.stones.end()) return group.id;
			}
			return -1;
		}

		std::set<int> getIsolated(const Board& board) const { // returns the ID of the isolated groups (no liberties);
			std::set<int> mySet;
			for (const auto& pair : groups) {
				const Group& group = pair.second;
				if (group.liberties(board).size() == 0) {
					mySet.insert(group.id);
				}
			}
			return mySet;
		}
	};

}
