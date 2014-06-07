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

#ifndef _HEAP_H_
#define _HEAP_H_

#include <algorithm>
#include <vector>

/*
 * Heap binário razoavelmente genérico. Tipo genérico de elemento, função ou
 * functor especificado no construtor e no template, funções ou functors que
 * devem ser especificados no template para obter ou alterar o índice no heap
 * de um elemento. Uma alternativa é usar um hashmap para fazer a mesma coisa.
 */
template <typename T, typename Compare, typename GetIndex, typename SetIndex>
class Heap {
public:
	Heap(Compare const &c) : cmp(c) {
		elements.reserve(1000);
	}

	// Organiza os elementos de modo a criar um heap.
	void heapify() {
		for (size_t ii = (elements.size() >> 1); ii > 0; ii--)
			heapify(ii - 1);
	}

	/*
	 * Insere um elemento no heap sem se preocupar em manter a propriedade de 
	 * heap. Requer uso posterior de heapify no elemento adicionado (ou o uso de
	 * heapify para organizar todos elementos).
	 */
	void insert_unsorted(T *elem) {
		setid(elem, elements.size());
		elements.push_back(elem);
	}

	// Insere um elemento no heap e o move para o local adequado.
	void insert(T *elem) {
		insert_unsorted(elem);
		update_elem(elem);
	}

	/*
	 * Retorna o elemento extremo do heap (que pode ser o maior ou o menor,
	 * dependendo da função de comparação suprida na construção do heap).
	 * O elemento é removido do heap, que então sofre uma reorganização.
	 */
	T *extract() {
		if (elements.empty()) {
			return 0;
		}

		// Modo "burro" de fazer: pega o último elemento do vetor e coloca na
		// frente, então reorganiza.
		T *elem = elements[0];
		T *repl = elements[0] = elements.back();
		elements.pop_back();

		// Se só tinha um elemento, as operações acima todas funcionam sem nenhum
		// problema; mas vamos sair agora, pois as demais iriam falhar.
		if (elements.empty()) {
			return elem;
		}

		setid(repl, 0);
		heapify(0);
		return elem;
	}

	// Assume que elem está no heap.
	void update_elem(T *elem) {
		size_t elemid = getid(elem);
		size_t parent = get_parent(elemid);

		// Move o elemento para a posição adequada.
		while (elemid > 0 && cmp(elements[elemid], elements[parent]))
		{
			std::swap(elements[parent], elements[elemid]);
			setid(elements[parent], elemid);
			setid(elem, parent);
			elemid = parent;
			parent = get_parent(elemid);
		}
	}

	bool empty() const {
		return elements.empty();
	}

protected:
	// Funções auxiliares.
	static inline size_t get_parent(size_t elem) {	return (elem - 1) >> 1;	};
	static inline size_t get_left(size_t elem)   {	return (elem << 1) + 1;	};
	static inline size_t get_right(size_t elem)  {	return (elem << 1) + 2;	};

	// Organiza os descendentes de elem de modo que elem vira a raiz de um heap.
	void heapify(size_t elem) {
		size_t ll = get_left(elem), rr = get_right(elem), smallest = elem;
	
		if (ll < elements.size() && cmp(elements[ll], elements[elem]))
			smallest = ll;
	
		if (rr < elements.size() && cmp(elements[rr], elements[smallest]))
			smallest = rr;
	
		if (smallest != elem)
		{
			// O elemento está na posição errada; mova-o para uma menos errada.
			std::swap(elements[smallest], elements[elem]);
			setid(elements[smallest], smallest);
			setid(elements[elem], elem);

			// Continue recursivamente até o elemento chegar ao local certo.
			heapify(smallest);
		}
	}

private:
	std::vector<T *> elements;
	Compare cmp;
	GetIndex getid;
	SetIndex setid;
};

#endif // _HEAP_H_
