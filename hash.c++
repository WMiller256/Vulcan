#include "hash.h"
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
	for (int ii = 0; ii < size; ii ++) {
		if (table[ii] != NULL) {
			int kk = ii;
			while (table[kk] == NULL) {
				kk++;
				if (kk == size) {
					break;
				}
			}
			if (kk < size) {
				table[ii] -> next = table[kk];
			}
			else {
				error("Hash indexing failure - line "+cyan+"%d"+res+" in file "+cyan+"%s"+res, __LINE__, __FILE__);
			}
		}
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

Force* Hash::force(CBody* target) {
	nodeptr current;
	Force* net = new Force();
	CBody* body;
	double fmagnitude;
	for (int ii = 0; ii < nbodies; ii ++) {
		current = table[ii];
		if (current != NULL) {
			for (int kk = 0; kk < current -> depth; kk ++) {
				body = current -> bodies.at(kk);
				if (body != target) {
					fmagnitude = (G * body -> Mass() * target -> Mass()) / pow(target -> distance(body), 2);

					println(in("CBody", "force")+"Magnitude of force between "+body -> Name()+" and "+
						target -> Name()+" is "+std::to_string(fmagnitude), 4);
				}
			}			
		}
	}
}
