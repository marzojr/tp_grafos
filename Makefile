# Binário e arquivo zip/tar
BIN := dijkstra
DISTFILE := TPDijkstra_Marzo_Tulio

# Diretórios de código fonte
SRCDIRS := .
# Diretórios de documentação
DOCDIRS := .

# Compilador
CXX = g++

# Compactador
ZIP = zip

# Conversor odt -> pdf
ODT2PDF = unoconv

# Arquivos fonte
SRCSCXX        := $(foreach SRCDIR,$(SRCDIRS),$(wildcard $(SRCDIR)/*.cc))
SRCSH          := $(foreach SRCDIR,$(SRCDIRS),$(wildcard $(SRCDIR)/*.h))
SRCDOCS        := $(foreach DOCDIR,$(DOCDIRS),$(wildcard $(DOCDIR)/*.odt))

# Arquivos de objeto
OBJECTS        := $(SRCSCXX:%.cc=%.o)
DEPENDENCIES   := $(OBJECTS:%.o=%.d)
DOCS           := $(SRCDOCS:%.odt=%.pdf)

# Variáveis para compilação
CXXFLAGS := -O2 -s -Wall -Wextra -MMD
CPPFLAGS := -D'BINNAME="$(BIN)"'
INCFLAGS := 
LDFLAGS := -Wl,-rpath,/usr/local/lib
LIBS := 

# Alvos
all: $(BIN)
	
docs: $(DOCS)

time: CPPFLAGS += -DLOGTIME
time: clean $(BIN)

zip: docs
	rm -f $(DISTFILE).zip
	zip -9 $(DISTFILE).zip Makefile $(SRCSCXX) $(SRCSH) $(DOCS)

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o *~ $(BIN) *.d *.zip

distclean: clean
	rm -f *.pdf

.PHONY: all count clean distclean time zip tar docs

# Regras de construção
.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

$(BIN): $(OBJECTS) $(OBJECTS)
	$(CXX) -o $(BIN) $(OBJECTS) $(LDFLAGS) $(LIBS)

%.o: %.cc
	$(CXX) -o $@ -c $(CXXFLAGS) $(CPPFLAGS) $< $(INCFLAGS)

%.pdf: %.odt
	$(ODT2PDF) -f pdf $<

# Dependências
-include $(DEPENDENCIES)

