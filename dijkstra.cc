/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * Copyright (C) 2014 Marzo Sette Torres Junior <marzojr@dcc.ufmg.br>
 *
 * TP is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TP is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "graph.h"
#include "heap.h"

#include <iostream>
#include <list>

using namespace std;

#if defined(__GNUC__) 
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#else
# define UNUSED(x) x 
#endif

/*
 * Functor de comparação para algoritmo de Dijkstra. Como o heap na STL é um
 * max-heap, queremos que ele compare na ordem inversa.
 */
struct DijkstraCmp {
	bool operator()(Node const *lhs, Node const *rhs) {
		return lhs->get_distance() < rhs->get_distance();
	}
};

/*
 * Functor de comparação para A*. Como o heap na STL é um max-heap, queremos que
 * ele compare na ordem inversa.
 */
struct AstarCmp {
	AstarCmp(Node const *dest) : target(dest) {		}

	bool operator()(Node const *lhs, Node const *rhs) {
		int dlhs = lhs->distance_to(target), drhs = rhs->distance_to(target);
		return lhs->get_distance() + dlhs < rhs->get_distance() + drhs;
	}
private:
	Node const *target;
};

// Functor para obter índice dos vértices.
struct GetIndex {
	size_t operator() (Node const *node) {
		return node->get_heapindex();
	}
};

// Functor para modificar índice dos vértices.
struct SetIndex {
	void operator() (Node *node, size_t index) {
		node->set_heapindex(index);
	}
};

/*
 * Functor que insere os vizinhos no heap para Dijkstra e A*.
 */
struct DijkstraSuccessors {
	template <typename H>
	void operator()(Node *node, Node *UNUSED(src), Node *UNUSED(dst), Graph &g, H &heap,
	                size_t &ins, size_t &upd) {
		vector<Node *> adj = g.get_adjacent_list(node);
		for (vector<Node *>::iterator it = adj.begin(); it != adj.end(); ++it) {
			Node *next = *it;
			unsigned dst = node->get_distance() + node->distance_to(next);
			if (next->get_distance() > dst) {
				next->set_distance(dst);
				next->set_parent(node);
				if (next->still_unseen()) {
					next->mark_seen();
					heap.insert(next);
					ins++;
				} else {
					heap.update_elem(next);
					upd++;
				}
			}
		}
	}
};

/*
 * Functor que insere os vizinhos no heap para Jump Point Search.
 */
struct JPSSuccessors {
	template <typename H>
	void operator()(Node *node, Node *src, Node *dst, Graph &g, H &heap,
	                size_t &ins, size_t &upd) {
		// Precisamos de saber a direção também, de modo que não dá para
		// usar Graph::get_adjacent_list.
		static Direction const dirs[] = {eNorth, eNorthEast, eEast, eSouthEast,
			                             eSouth, eSouthWest, eWest, eNorthWest};
		vector<Node *> adj;
		if (node == src) {
			for (unsigned ii = 0; ii < sizeof(dirs) / sizeof(dirs[0]); ii++) {
				add_neighbour(g, node, dirs[ii], adj);
			}
		} else {
			adj = get_neighbours(node, g);
		}
		for (vector<Node *>::iterator it = adj.begin(); it != adj.end(); ++it) {
			Node *next = *it;
			Direction dir = next->get_dir_from();
			next = jump(node, src, dst, dir, g);
			if (next) {
				unsigned dst = node->get_distance() + node->distance_to(next);
				if (next->get_distance() > dst) {
					next->set_dir_from(dir);
					next->set_distance(dst);
					next->set_parent(node);
					//if (next->still_unseen()) {
					if (!next->already_seen()) {
						next->mark_seen();
						heap.insert(next);
						ins++;
					} else {
						heap.update_elem(next);
						upd++;
					}
				}
			}
		}
	}
private:
	Node *jump(Node *node, Node *src, Node *dst, Direction dir, Graph &g) {
		Node *next = node;
		do {
			next = g.get_adjacent(next, dir);
			if (!next) {
				return 0;
			} else if (next == dst) {
				return next;
			}
			
			// O nó tem vizinhos forçados na sua vizinhança?
			vector<Node *> adj;
			forced_neighbours(g, next, dir, adj);
			if (!adj.empty()) {
				return next;
			}

			// Recursão nas diagonais.
			switch (dir) {
				case eNorthEast:
					if (jump(next, src, dst, eNorth, g) != 0) {
						return next;
					}
					if (jump(next, src, dst, eEast, g) != 0) {
						return next;
					}
					break;
				case eSouthEast:
					if (jump(next, src, dst, eSouth, g) != 0) {
						return next;
					}
					if (jump(next, src, dst, eEast, g) != 0) {
						return next;
					}
					break;
				case eSouthWest:
					if (jump(next, src, dst, eSouth, g) != 0) {
						return next;
					}
					if (jump(next, src, dst, eWest, g) != 0) {
						return next;
					}
					break;
				case eNorthWest:
					if (jump(next, src, dst, eNorth, g) != 0) {
						return next;
					}
					if (jump(next, src, dst, eWest, g) != 0) {
						return next;
					}
					break;
				default:
					break;
			}
		} while (1);
	}

	void add_neighbour(Graph &g, Node *node, Direction dir, vector<Node *> &adj) {
		Node *next = g.get_adjacent(node, dir);
		if (next) {
			adj.push_back(next);
			next->set_dir_from(dir);
		}
	}

	void natural_neighbours(Graph &g, Node *node, Direction dir, vector<Node *> &adj) {
		// Vizinho natural comum a todos casos.
		add_neighbour(g, node, dir, adj);

		switch (dir) {
			case eNorthEast:
				// Naturais:
				add_neighbour(g, node, eNorth, adj);
				add_neighbour(g, node, eEast, adj);
				break;
			case eSouthEast:
				// Naturais:
				add_neighbour(g, node, eSouth, adj);
				add_neighbour(g, node, eEast, adj);
				break;
			case eSouthWest:
				// Naturais:
				add_neighbour(g, node, eSouth, adj);
				add_neighbour(g, node, eWest, adj);
				break;
			case eNorthWest:
				// Naturais:
				add_neighbour(g, node, eNorth, adj);
				add_neighbour(g, node, eWest, adj);
				break;
			default:
				break;
		}
	}

	void forced_neighbours(Graph &g, Node *node, Direction dir, vector<Node *> &adj) {
		switch (dir) {
			case eEast:
				// Forçados:
				if (!g.get_adjacent(node, eNorth)) {
					add_neighbour(g, node, eNorthEast, adj);
				}
				if (!g.get_adjacent(node, eSouth)) {
					add_neighbour(g, node, eSouthEast, adj);
				}
				break;
			case eWest:
				// Forçados:
				if (!g.get_adjacent(node, eNorth)) {
					add_neighbour(g, node, eNorthWest, adj);
				}
				if (!g.get_adjacent(node, eSouth)) {
					add_neighbour(g, node, eSouthWest, adj);
				}
				break;
			case eNorth:
				// Forçados:
				if (!g.get_adjacent(node, eEast)) {
					add_neighbour(g, node, eNorthEast, adj);
				}
				if (!g.get_adjacent(node, eWest)) {
					add_neighbour(g, node, eNorthWest, adj);
				}
				break;
			case eSouth:
				// Forçados:
				if (!g.get_adjacent(node, eEast)) {
					add_neighbour(g, node, eSouthEast, adj);
				}
				if (!g.get_adjacent(node, eWest)) {
					add_neighbour(g, node, eSouthWest, adj);
				}
				break;
			case eNorthEast:
				// Forçados:
				if (!g.get_adjacent(node, eWest)) {
					add_neighbour(g, node, eNorthWest, adj);
				}
				if (!g.get_adjacent(node, eSouth)) {
					add_neighbour(g, node, eSouthEast, adj);
				}
				break;
			case eSouthEast:
				// Forçados:
				if (!g.get_adjacent(node, eWest)) {
					add_neighbour(g, node, eSouthWest, adj);
				}
				if (!g.get_adjacent(node, eNorth)) {
					add_neighbour(g, node, eNorthEast, adj);
				}
				break;
			case eSouthWest:
				// Forçados:
				if (!g.get_adjacent(node, eEast)) {
					add_neighbour(g, node, eSouthEast, adj);
				}
				if (!g.get_adjacent(node, eNorth)) {
					add_neighbour(g, node, eNorthWest, adj);
				}
				break;
			case eNorthWest:
				// Forçados:
				if (!g.get_adjacent(node, eEast)) {
					add_neighbour(g, node, eNorthEast, adj);
				}
				if (!g.get_adjacent(node, eSouth)) {
					add_neighbour(g, node, eSouthWest, adj);
				}
				break;
			default:
				break;
		}
	}

	vector<Node *> get_neighbours(Node *node, Graph &g) {
		vector<Node *> adj;
		adj.reserve(8);
		Direction dir = node->get_dir_from();
		natural_neighbours(g, node, dir, adj);
		forced_neighbours(g, node, dir, adj);
		return adj;
	}
};

// Versão genérica para Dijkstra, A* e JPS usando functors ou poiteiros para
// funções para efetuar as operações necessárias.
template <typename Compare, typename Successors>
void ShortestPath(Graph &g, Node *src, Node *dst, Compare cmp, Successors succ,
                  size_t &ins, size_t &upd, size_t &pop) {
	g.init_single_source();
	ins = upd = pop = 0;

	Heap<Node, Compare, GetIndex, SetIndex> heap(cmp);
	heap.insert(src);
	ins++;

	while (!heap.empty()) {
		Node *u = heap.extract();
		pop++;
		u->mark_done();

		if (u == dst) {
			break;
		}

		succ(u, src, dst, g, heap, ins, upd);
	}
}

void dump_path_info(Graph &UNUSED(g), Node *src, Node *dst, char const *method, size_t ins, size_t upd, size_t pop) {
	cout << method << endl;
	cout << "insert = " << ins << ", update = " << upd << ", extract = " << pop << endl;
	if (dst->get_parent() == 0) {
		cout << "destination unreachable from source" << endl;
		return;
	}
	cout << "distance = " << dst->get_distance() / DISTANCE_PRECISION << endl;
	cout << "path:" << endl;
	list<Node *> path;
	Node *prev = dst;
	do {
		path.push_front(prev);
		prev = prev->get_parent();
	} while (prev != src);

	path.push_front(src);

	size_t nodecnt = 16;
	for (list<Node *>::const_iterator it = path.begin(); it != path.end(); ++it) {
		if (++nodecnt == 16) {
			cout << endl << "\t";
			nodecnt = 0;
		}
		Node *curr = *it;
		cout << "(" << curr->get_x() << ", " << curr->get_y() << "); ";
	}
	if (nodecnt != 0) {
		cout << endl;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cerr << "Falta nome do grafo." << endl;
		return 1;
	}

	Graph g(argv[1]);
	if (!g.is_valid()) {
		cerr << "Grafo '" << argv[1] << "' invalido ou inexistente." << endl;
		return 1;
	}

	Node *src = g.get_src(), *dst = g.get_dst();
	// Para estatísticas.
	size_t ins, upd, pop;
	// Dijkstra
	ShortestPath(g, src, dst, DijkstraCmp(), DijkstraSuccessors(), ins, upd, pop);
	dump_path_info(g, src, dst, "==== Dijkstra ====", ins, upd, pop);

	// A*
	ShortestPath(g, src, dst, AstarCmp(dst), DijkstraSuccessors(), ins, upd, pop);
	dump_path_info(g, src, dst, "==== A* ==========", ins, upd, pop);

	// JPS
	ShortestPath(g, src, dst, AstarCmp(dst), JPSSuccessors(), ins, upd, pop);
	dump_path_info(g, src, dst, "==== JPS =========", ins, upd, pop);
	return 0;
}



