# Variáveis
CXX = g++
CXXFLAGS = -I./include
SRCDIR = ./src
OBJDIR = ./obj
TARGET = MOVRPDD

# Lista de arquivos fonte
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Regra padrão
all: $(TARGET)

# Regra para criar o executável
$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^

# Regra para criar arquivos objeto
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -rf $(OBJDIR) $(TARGET)

# Regra para recompilar tudo
rebuild: clean all
