# Variáveis
CXX = g++
CXXFLAGS = -I./include
DEBUGFLAGS = -g -O0 -I./include
SRCDIR = ./src
OBJDIR = ./obj
TARGET = MOVRPDD
DEBUG_TARGET = MOVRPDD-debug

# Lista de arquivos fonte
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEBUG_OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/debug_%.o)

# Regra padrão
all: $(TARGET)

# Regra para criar o executável normal
$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^

# Regra para criar arquivos objeto normais
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para criar o executável de debug
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CXX) -o $@ $^

$(OBJDIR)/debug_%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(DEBUGFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -rf $(OBJDIR) $(TARGET) $(DEBUG_TARGET)

# Regra para recompilar tudo
rebuild: clean all
