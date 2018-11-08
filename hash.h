/*
 * hash.h
 *
 * William Miller
 * Nov 8, 2018
 *
 * class definition for (Hash), a hash table with nodes containing (CBody) type objects
 *
 *
 */

#ifndef HASH_H
#define HASH_H

#include <iostream>
#include <string>
#include <vector>

#include "force.h"
#include "pos.h"

class CBody;

class Hash
{
private:
	typedef struct node {
		int depth;					// Number of bodies in this block
		int key;					// The hash key
		node* next;					// Pointer to the next node, for faster iterating
		std::vector<CBody*> bodies;	// Other bodies in the given hash block
	}* nodeptr; 
	nodeptr node_search(Pos pos);

public:
	Hash(int n = 0);				// Constructor

	nodeptr* table;					// Table of pointers to all the nodes
	long hash_f(Pos pos);			// Hash function

	void addNode(CBody* body);		// Adds a node to the 	
	CBody* find(Pos pos);			// Returns a pointer to the (CBody) at the given (Pos) {pos}

	void write(const std::string& filename);	// Defined in simio.c++

	Force* force(CBody* target);	// Calculates the force on the given (CBody) {target}
	
	int size;
};

#endif // HASH_H
