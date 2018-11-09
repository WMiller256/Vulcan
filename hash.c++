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
	
	println(in("Hash", "hash_f")+" - \n  {ret}: "+scientific(ret)+"\n  {minradius}: "+
		scientific(minradius)+"\n  {pos.originDist()}: "+scientific(odist)+
		"\n  {blockwidth}: "+scientific(blockwidth), 5);
	if (ret < 0) {
		println(red+white_back+" Error "+res+" hash function returned "+std::to_string(ret));
		return 0;
	}
	return ret;
}

void Hash::addNode(CBody* body) {
	print("Adding new "+cyan+bright+"Hash"+res+" node...  ");
	print("\n", 2);
	println(in("Hash", "addNode")+"    "+body -> pos().info(), 3);
	printrln(in("Hash", "addNode")+"    Origin distance: ", scientific(body -> originDist()), 4);
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
	println("  Hash value: "+red+bright+std::to_string(hash_v)+res, 3);
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

void Hash::printForces() {
	int gdebug = debug;
	nodeptr current;
	nodeptr target;
	CBody* body;
	CBody* tbody;
	double fmagnitude;
	for (int jj = 0; jj < nbodies; jj ++) {
		target = table[jj];
		if (target != NULL) {
			for (int mm = 0; mm < target -> depth; mm ++) {
				tbody = target -> bodies.at(mm);	
				for (int ii = 0; ii < nbodies; ii ++) {
					current = table[ii];
					if (current != NULL) {
						int depth = current -> depth;
						for (int kk = 0; kk < depth; kk ++) {
							body = current -> bodies.at(kk);
							if (body != tbody) {
								debug = -1;
								fmagnitude = (G * body -> Mass() * tbody -> Mass()) / pow(tbody -> distance(body), 2);

								debug = gdebug;
								printrln(in("CBody", "force")+"   Magnitude of force between "+body -> Name()+" and "+
									tbody -> Name()+" is ", scientific(fmagnitude));
							}
						}
					}
				}
			}
		}
	}
	debug = gdebug;
}

Force* Hash::force(CBody* target) {
	nodeptr current;
	Force* net = new Force();
	CBody* body;
	double fmagnitude;
	vec dir;
	vec v;
	for (int ii = 0; ii < nbodies; ii ++) {
		current = table[ii];
		if (current != NULL) {
			int depth = current -> depth;
			for (int kk = 0; kk < depth; kk ++) {
				body = current -> bodies.at(kk);
				if (body != target) {
					fmagnitude = (G * body -> Mass() * target -> Mass()) / pow(target -> distance(body), 2);
					dir = body -> pos().direction(body -> COM(target));
					v = dir * fmagnitude;
					*net += v;

					printrln(in("Hash", "force")+"    Magnitude of force between "+body -> Name()+" and "+
						target -> Name()+" is ", scientific(fmagnitude), 4);
					printrln(in("Hash", "force")+"    Direction of force between "+body -> Name()+" and "+
						target -> Name()+" is ", dir.info(), 4);
					printrln(in("Hash", "force")+"    Force vector between "+body -> Name()+" and "+
						target -> Name()+" is ", v.info(), 4);
					printrln(in("Hash", "force")+"    Net force vector on "+body -> Name()+" is ", net -> info(), 4);
				}
			}			
		}
	}
}

void Hash::step() {
	nodeptr current;
	CBody* body;
	for (int ii = 0; ii < nbodies; ii ++) {
		current = table[ii];
		if (current != NULL) {
			int depth = current -> depth;
			for (int kk = 0; kk < depth; kk ++) {
				body = current -> bodies.at(kk);
				force(body);
			}
		}
	}
}
