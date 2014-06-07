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

#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <cmath>
#include <cstdlib>

#define DISTANCE_PRECISION 10.0

// Direções usadas em JPS.
enum Direction {
	eNorth,
	eNorthEast,
	eEast,
	eSouthEast,
	eSouth,
	eSouthWest,
	eWest,
	eNorthWest
};

/*
 * Nó no grafo. Guarda um mundo de informações que, em uma aplicação geral,
 * seriam melhor armazenadas em estruturas temporárias separadas usadas pelos
 * algoritmos.
 */
class Node {
public:
	friend class Graph;
	friend class std::vector<Node>;

	// Uso semelhante às buscas em largura e profundidade.
	enum Color {
		eWhite,
		eGray,
		eBlack
	};

	Node(short _x, short _y, bool _blocked)
		: parent(0), dist(~0u), clr(eWhite), x(_x), y(_y), blocked(_blocked) {
	}

	// Prepara para começar tudo de novo.
	void init_single_source() {
		dist = ~0u;
		clr = eWhite;
		parent = 0;
	}

	// Cálculo de distância usando métrica Euclideana padrão.
	unsigned distance_to(Node const *other) const {
		int dx = x - other->x, dy = y - other->y;
		return static_cast<unsigned>(DISTANCE_PRECISION * sqrt(dx * dx + dy * dy));
	}

	// Getters.
	short get_x() const             {	return x;	}
	short get_y() const             {	return y;	}
	bool is_blocked() const         {	return blocked;	}
	unsigned get_distance() const   {	return dist;	}
	bool still_unseen() const       {	return clr == eWhite;	}
	bool already_seen() const       {	return clr == eGray;	}
	bool already_done() const       {	return clr == eBlack;	}
	Node *get_parent() const        {	return parent;	}
	size_t get_heapindex() const    {	return heapindex;	}
	Direction get_dir_from() const  {	return from;	}

	// Setters.
	void set_distance(unsigned dst) {	dist = dst;	}
	void mark_unseen()              {	clr = eWhite;	}
	void mark_seen()                {	clr = eGray;	}
	void mark_done()                {	clr = eBlack;	}
	void set_parent(Node *p)        {	parent = p;	}
	void set_heapindex(size_t v)    {	heapindex = v;	}
	void set_dir_from(Direction f)  {	from = f;	}

protected:
	Node()
		: parent(0), dist(~0u), clr(eWhite) {
	}

	// Usado durante a inicialização do grafo, para que cada nó saiba sua
	// posição na grade 2d.
	void init(short _x, short _y, bool _blocked) {
		x = _x;
		y = _y;
		blocked = _blocked;
	}

private:
	// Informações para Dijkstra, A* e JPS:
	Node *parent;
	size_t heapindex;	
	unsigned dist;
	Color clr;
	// Informações para JPS:
	Direction from;
	// Informações do vértice em si.
	short x, y;
	bool blocked;
};

/*
 * Classe de grafo geral. Uma aplicação mais real provavelmente usaria algo mais
 * flexível, que pudesse, por exemplo, carregar apenas parte do mapa. E também
 * algo que tivesse mais funcionalidade, ao invés de ser tão específico para
 * Dijkstra, A* e JPS.
 */
class Graph {
public:
	Graph(char const *fname);

	/*
	 * Se o grafo lido é válido ou não: precisa ter pelo menos 2 nós, um dos
	 * quais é fonte e o outro é destino.
	 */
	bool is_valid() const {
		return w != 0 && h != 0 && src != 0 && dst != 0;
	}

	// Nó de origem.
	Node       *get_src()       {	return src;	}

	// Nó de destino.
	Node       *get_dst()       {	return dst;	}

	/*
	 * Retorna todos nós adjacentes ao nó dado. Os nós adjacentes são obtidos
	 * pela função get_adjacent.
	 */
	std::vector<Node *> get_adjacent_list(Node const *node) {
		return get_adjacent_list(node->get_x(), node->get_y());
	}

	/*
	 * Retorna o nó adjacente ao nó dado na direção dada se o nó final:
	 * (1) estiver dentro da grade;
	 * (2) não for um nó bloqueado;
	 * (3) puder ser alcançado do nó de origem (basicamente, diagonais tem que
	 *     obedecer certas restrições).
	 */
	Node *get_adjacent(Node const *node, Direction dir) {
		return get_adjacent(node->get_x(), node->get_y(), dir);
	}

	// Prepara o grafo para executar uma busca por melhor caminho.
	void init_single_source() {
		for (std::vector<Node>::iterator it = nodes.begin();
		     it != nodes.end(); ++it) {
			it->init_single_source();
		};
		src->set_distance(0);
	}

private:
	unsigned w, h;
	std::vector<Node> nodes;
	Node *src, *dst;

	/*
	 * Retorna o ponteiro de um nó dado suas coordenadas, com verificação para
	 * garantir que o nó referenciado é válido. Retorna 0 para um nó fora dos
	 * limites.
	 */
	Node *get_node(int x, int y) {
		if (x < 0 || static_cast<unsigned>(x) >= w
		    || y < 0 || static_cast<unsigned>(y) >= h) {
			return 0;
		} else {
			return &(nodes[w * y + x]);
		}
	}

	/*
	 * Retorna todos nós adjacentes ao nó dado. Os nós adjacentes são obtidos
	 * pela função get_adjacent.
	 */
	std::vector<Node *> get_adjacent_list(int x, int y) {
		std::vector<Node *> nodes;
		nodes.reserve(8);
		static Direction const dirs[] = {eNorth, eNorthEast, eEast, eSouthEast,
		                                 eSouth, eSouthWest, eWest, eNorthWest};

		for (unsigned ii = 0; ii < sizeof(dirs) / sizeof(dirs[0]); ii++) {
			Node *node = get_adjacent(x, y, dirs[ii]);
			if (node) {
				nodes.push_back(node);
			}
		}

		return nodes;
	}

	/*
	 * Retorna o nó adjacente ao nó dado na direção dada se o nó final:
	 * (1) estiver dentro da grade;
	 * (2) não for um nó bloqueado;
	 * (3) puder ser alcançado do nó de origem (basicamente, diagonais tem que
	 *     obedecer certas restrições).
	 */
	Node *get_adjacent(int x, int y, Direction dir) {
		Node *adj = 0;
		if (!can_step_to(x, y, dir)) {
			return adj;
		}

		switch (dir) {
			case eNorth:
				adj = get_node(x + 0, y - 1);
				break;
			case eNorthEast:
				adj = get_node(x + 1, y - 1);
				break;
			case eEast:
				adj = get_node(x + 1, y + 0);
				break;
			case eSouthEast:
				adj = get_node(x + 1, y + 1);
				break;
			case eSouth:
				adj = get_node(x + 0, y + 1);
				break;
			case eSouthWest:
				adj = get_node(x - 1, y + 1);
				break;
			case eWest:
				adj = get_node(x - 1, y + 0);
				break;
			case eNorthWest:
				adj = get_node(x - 1, y - 1);
				break;
		}

		if (!adj || adj->is_blocked()) {
			return 0;
		} else {
			return adj;
		}
	}

	// Filtra diagonais que não tenham pelo menos uma direção adjacente que
	// não seja bloqueada.
	bool can_step_to(int x, int y, Direction to) {
		switch (to) {
			case eEast:
			case eWest:
			case eNorth:
			case eSouth:
				return true;
			case eNorthEast:
				return get_adjacent(x, y, eEast) || get_adjacent(x, y, eNorth);
			case eSouthEast:
				return get_adjacent(x, y, eEast) || get_adjacent(x, y, eSouth);
			case eSouthWest:
				return get_adjacent(x, y, eWest) || get_adjacent(x, y, eSouth);
			case eNorthWest:
				return get_adjacent(x, y, eWest) || get_adjacent(x, y, eNorth);
			default:
				return false;
		}
	}
};

#endif // _GRAPH_H_

