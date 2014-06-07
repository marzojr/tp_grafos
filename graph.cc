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
#include "bitmap.h"

#include <iostream>
#include <fstream>

using namespace std;

Graph::Graph(char const *fname) {
	// Grafo inválido.
	w = h = 0;
	src = dst = 0;
	ifstream fin(fname, ios::binary|ios::in);
	if (!fin.good()) {
		return;
	}

	// Cabeçalho de arquivo.
	BITMAPFILEHEADER fhdr;
	fin.read(reinterpret_cast<char *>(&fhdr), sizeof(fhdr));
	if (fhdr.bfType != 0x4D42) {
		return;
	}

	// Cabeçalho de bitmap.
	BITMAPINFOHEADER ihdr;
	fin.read(reinterpret_cast<char *>(&ihdr), sizeof(ihdr.biSize));
	fin.seekg(-sizeof(ihdr.biSize), ios::cur);
	fin.read(reinterpret_cast<char *>(&ihdr), ihdr.biSize);
	if (ihdr.biPlanes != 1 || (ihdr.biBitCount != 4 && ihdr.biBitCount != 8) ||
	    ihdr.biCompression != BI_RGB || ihdr.biClrUsed != 4) {
		return;
	}

	// Tabela de cores.
	fin.seekg(sizeof(fhdr) + ihdr.biSize);
	RGBQUAD *colors = new RGBQUAD[ihdr.biClrUsed];
	fin.read(reinterpret_cast<char *>(colors), ihdr.biClrUsed * sizeof(RGBQUAD));
	int normalnode = -1, blockednode = -1, startnode = -1, endnode = -1;
	for (unsigned ii = 0; ii < ihdr.biClrUsed; ii++) {
		RGBQUAD &clr = colors[ii];
		if (clr.rgbBlue < 32 && clr.rgbGreen < 32 && clr.rgbRed < 32) {
			blockednode = ii;
		} else if (clr.rgbBlue >= 224 && clr.rgbGreen >= 224 && clr.rgbRed >= 224) {
			normalnode = ii;
		} else if (clr.rgbBlue < 32 && clr.rgbGreen >= 224 && clr.rgbRed < 32) {
			startnode = ii;
		} else if (clr.rgbBlue < 32 && clr.rgbGreen < 32 && clr.rgbRed >= 224) {
			endnode = ii;
		}
	}
	delete [] colors;

	if (normalnode < 0) {
		cerr << "Falta uma cor branca no bitmap do grafo." << endl;
		return;
	}

	if (blockednode < 0) {
		cerr << "Falta uma cor preta no bitmap do grafo." << endl;
		return;
	}

	if (startnode < 0) {
		cerr << "Falta uma cor verde no bitmap do grafo." << endl;
		return;
	}

	if (endnode < 0) {
		cerr << "Falta uma cor vermelha no bitmap do grafo." << endl;
		return;
	}

	w = ihdr.biWidth;
	h = ihdr.biHeight;
	nodes.resize(w * h);

	// Pixels.
	fin.seekg(fhdr.bfOffBits);
	for (unsigned jj = 0; jj < h && fin.good(); jj++) {
		// Bitmaps tem linhas em ordem invertida.
		unsigned yy = h - jj - 1;
		for (unsigned ii = 0; ii < w && fin.good(); ) {
			int pix = fin.get();
			if (ihdr.biBitCount == 8) {
				Node *curr = &(nodes[w * yy + ii]);
				curr->init(ii, yy, pix == blockednode);
				if (pix == startnode) {
					src = curr;
				} else if (pix == endnode) {
					dst = curr;
				}
				ii++;
			} else {
				for (unsigned kk = 0; kk < 2; kk++, pix <<= 4) {
					Node *curr = &(nodes[w * yy + ii]);
					int pixval = (pix & 0xF0) >> 4;
					curr->init(ii, yy, pixval == blockednode);
					if (pixval == startnode) {
						src = curr;
					} else if (pixval == endnode) {
						dst = curr;
					}
					ii++;
				}
			}
		}
	}
}
