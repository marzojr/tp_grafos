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
	void operator()(Node *node, Graph &g, H &heap, size_t &ins, size_t &upd) {
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
	void operator()(Node *node, Graph &g, H &heap, size_t &ins, size_t &upd) {
	}
};

// Versão genérica para Dijkstra, A* e JPS usando functors ou poiteiros para
// funções para efetuar as operações necessárias.
template <typename Compare, typename Successors>
void ShortestPath(Graph &g, Compare cmp, Successors succ, size_t &ins, size_t &upd, size_t &pop) {
	g.init_single_source();
	ins = upd = pop = 0;

	Node *src = g.get_src(), *dst = g.get_dst();
	
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

		succ(u, g, heap, ins, upd);
	}
}

void dump_path_info(Graph &g, char const *method, size_t ins, size_t upd, size_t pop, bool interp) {
	cout << method << endl;
	cout << "insert = " << ins << ", update = " << upd << ", extract = " << pop << endl;
	Node *dest = g.get_dst();
	if (dest->get_parent() == 0) {
		cout << "destination unreachable from source" << endl;
		return;
	}
	cout << "distance = " << dest->get_distance() / 10.0 << endl;
	cout << "path:" << endl;
	list<Node *> path;
	Node *prev = dest, *src = g.get_src();
	do {
		path.push_front(prev);
		if (interp) {
			Node *next = prev->get_parent();
			Direction dir = flip_dir(prev->get_dir_from());
			prev = g.get_adjacent(prev, dir);
			while (prev != next) {
				path.push_front(prev);
				prev = g.get_adjacent(prev, dir);
			}
		} else {
			prev = prev->get_parent();
		}
	} while (prev != src);

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

	// Para estatísticas.
	size_t ins, upd, pop;
	// Dijkstra
	ShortestPath(g, DijkstraCmp(), DijkstraSuccessors(), ins, upd, pop);
	dump_path_info(g, "==== Dijkstra ====", ins, upd, pop, false);

	// A*
	ShortestPath(g, AstarCmp(g.get_dst()), DijkstraSuccessors(), ins, upd, pop);
	dump_path_info(g, "==== A* ==========", ins, upd, pop, false);

	// JPS
#if 0
	ShortestPath(g, AstarCmp(g.get_dst()), JPSSuccessors(), ins, upd, pop);
	dump_path_info(g, "==== JPS =========", ins, upd, pop, true);
#endif
	return 0;
}



