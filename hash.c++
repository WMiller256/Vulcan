#include "csim.h"
#include "simio.h"

Hash::Hash(int n) {
	table = new nodeptr[n];
	size = n;
	blockwidth = (minradius - maxradius) / nbodies;
	if (blockwidth <= 0) {
		blockwidth = 1;
	}
	for (int ii = 0; ii < n; ii ++) {
		table[ii] = NULL;
	}
}

long Hash::hash_f(Pos pos) {
	double odist = pos.originDist();
	long ret = (odist - minradius) / blockwidth;
	println("In ["+bright+blue+"Hash::hash_f()"+res+"] - \n  {ret}: "+std::to_string(ret)+"\n  {minradius}: "+
		std::to_string(minradius)+"\n  {pos.originDist()}: "+std::to_string(odist)+
		"\n  {blockwidth}: "+std::to_string(blockwidth), 5);
	if (ret < 0) {
		println(red+white_back+" Error "+res+" hash function returned "+std::to_string(ret));
		return 0;
	}
	return ret;
}

void Hash::addNode(CBody* body) {
	print("Adding new "+cyan+bright+"Hash"+res+" node...  ");
	print("\n", 2);
	println("In ["+bright+blue+"Hash::addNode()"+res+"]     "+body -> pos().info(), 3);
	println("In ["+bright+blue+"Hash::addNode()"+res+"]     Origin distance: "+std::to_string(body -> originDist()), 4);
	double odist = body -> originDist();
	if (!(odist < 0)) {
		if (odist < minradius) {
			println("New {minradius}: "+magenta+std::to_string(odist)+res, 3);
			minradius = odist;
		}
		if (odist > maxradius) {
			println("New {maxradius}: "+magenta+std::to_string(odist)+res, 3);
			maxradius = odist;
		}
	}
	else {
		int warn;
		if (warnings) {
			warn = 0;
		}
		else {
			warn = -1;
		}
		println(blue+white_back+" Warning "+res+" [originDist()] returned "+std::to_string(odist), warn);
	}
	int hash_v = hash_f(body -> pos());
	println("Hash value: "+red+bright+std::to_string(hash_v)+res, 3);
	nodeptr target = table[hash_v];
	if (target == NULL) {				// Empty block case
		target = new node;
		target -> depth = 1;
		target -> key = hash_v;
		target -> bodies.push_back(body);
	}
	else {
		target -> bodies.push_back(body);
		target -> depth = target -> bodies.size();
	}
	table[hash_v] = target;
	println(green+"done"+res);
}

Hash::nodeptr Hash::node_search(Pos pos) {
	int hash_v = hash_f(pos);
	nodeptr target = target;
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
	print(pos.info(), 4);
	std::cout << " found ";
	return NULL;
}
