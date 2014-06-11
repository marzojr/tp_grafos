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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

Graph::Graph(char const *fname) {
	// Marca como grafo inválido.
	w = h = 0;

	ifstream fin(fname, ios::in);
	if (!fin.good()) {
		return;
	}

	string hdr;
	getline(fin, hdr);
	if (hdr != "type octile") {
		cerr << "Mapa '" << fname << "' invalido." << endl;
		return;
	}

	string sw, sh, sm;
	fin >> sh >> h >> sw >> w >> sm;

	if (!fin.good() || sh != "height" || sw != "width" || sm != "map") {
		w = h = 0;
		cerr << "Mapa '" << fname << "' invalido." << endl;
		return;
	}

	// Cria espaço para o grafo.
	nodes.resize(w * h);
	fin >> ws;

	// Vértices.
	string line;
	line.reserve(w + 1);
	for (unsigned jj = 0; jj < h && fin.good(); jj++) {
		getline(fin, line);
		for (unsigned ii = 0; ii < w && fin.good(); ii++) {
			char node = line[ii];
			Node *curr = &(nodes[w * jj + ii]);
			switch (node) {
				case '\n':
					// Ignore.
					break;
				case '.':	// Passável.
				case 'G':	// Passável.
					curr->init(ii, jj, false);
					break;
				case '@':	// Impassável.
				case 'O':	// Impassável.
				case 'T':	// Impassável.
				default:	// Inválido; vamos assumir impassável.
					curr->init(ii, jj, true);
					break;
			}
		}
	}
}
