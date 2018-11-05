#include "csim.h"

int blockwidth;

Hash::Hash(int n) {
	nodeptr* table = new nodeptr[n];
	size = n;
	blockwidth = (minradius - maxradius) / nbodies;
	if (blockwidth == 0) {
		blockwidth = 1;
	}
	for (int ii = 0; ii < n; ii ++) {
		table[ii] = NULL;
	}
}

int Hash::hash_f(Pos pos) {
	return (pos.originDist() -  minradius) / blockwidth;
}

void Hash::addNode(CBody* body) {
	int odist = body -> originDist();
	if (odist < minradius) {
		minradius = odist;
	}
	if (odist > maxradius) {
		maxradius = odist;
	}
	int hash_v = hash_f(body -> pos());
	nodeptr previous = NULL;
	nodeptr target = table[hash_v];
	if (target == NULL) {				// Empty block case
		target = new node;
		target -> depth = 1;
		target -> key = hash_v;
		target -> bodies.push_back(body);
		if (previous == NULL) {
			table[hash_v] = target;
		}
		else {
			target -> next = previous -> next;
			previous -> next = target;
		}
	}
	else {
		target -> bodies.push_back(body);
		target -> depth = target -> bodies.size();
	}
}

Hash::nodeptr Hash::node_search(Pos pos) {
	int hash_v = hash_f(pos);
	nodeptr target = table[hash_v];
	return target;
}

CBody* Hash::find(Pos pos) {
	nodeptr target = node_search(pos);
	std::vector<CBody*> tbodies = target -> bodies;
	int blocksize = target -> depth;
	for (int ii = 0; ii < blocksize; ii ++) {
		if (tbodies.at(ii) -> pos() == pos) {
			return tbodies.at(ii);
		}
	}
	std::cout << red+white_back << " Error " << black_back << " no body at position ";
	pos.print() ;
	std::cout << " found ";
	return NULL;
}
