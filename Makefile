# Variáveis
CXX = g++
CXXFLAGS = -I./include
DEBUGFLAGS = -g -O0 -I./include
PROFILEFLAGS = -pg -g -I./include # Flags para gprof (-pg) com símbolos de debug (-g)

SRCDIR = ./src
OBJDIR = ./obj

TARGET = MOVRPDD
DEBUG_TARGET = MOVRPDD-debug
PROFILE_TARGET = MOVRPDD-profile

# Lista de arquivos fonte
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEBUG_OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/debug_%.o)
PROFILE_OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/profile_%.o)

# Regra padrão
all: $(TARGET)

# --- Regras para o executável normal ---
$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regras para o executável de debug ---
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CXX) -o $@ $^

$(OBJDIR)/debug_%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(DEBUGFLAGS) -c $< -o $@

# --- Regras para o executável de profiling (gprof) ---
profile: $(PROFILE_TARGET)

$(PROFILE_TARGET): $(PROFILE_OBJECTS)
	$(CXX) -pg -o $@ $^ # A flag -pg também é necessária na linkagem

$(OBJDIR)/profile_%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(PROFILEFLAGS) -c $< -o $@

# --- Limpeza ---
clean:
	rm -rf $(OBJDIR) $(TARGET) $(DEBUG_TARGET) $(PROFILE_TARGET) gmon.out *.txt

# --- Regras de conveniência ---
rebuild: clean all