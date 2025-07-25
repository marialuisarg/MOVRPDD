# Variáveis
CXX = g++
CXXFLAGS = -I./include
DEBUGFLAGS = -g -O0 -I./include
PROFILEFLAGS = -pg -g -I./include # Flags para gprof (-pg) com símbolos de debug (-g)
ASANFLAGS = -g -fsanitize=address -I./include # Flags para AddressSanitizer (-fsanitize=address) com debug (-g)

SRCDIR = ./src
OBJDIR = ./obj

TARGET = MOVRPDD
DEBUG_TARGET = MOVRPDD-debug
PROFILE_TARGET = MOVRPDD-profile
ASAN_TARGET = MOVRPDD-asan # Novo executável para o sanitizer

# Lista de arquivos fonte
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEBUG_OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/debug_%.o)
PROFILE_OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/profile_%.o)
ASAN_OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/asan_%.o) # Novos objetos para o sanitizer

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
	$(CXX) $(PROFILEFLAGS) -o $@ $^ 

$(OBJDIR)/profile_%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(PROFILEFLAGS) -c $< -o $@

# --- Regras para o executável com AddressSanitizer ---
asan: $(ASAN_TARGET)

$(ASAN_TARGET): $(ASAN_OBJECTS)
	$(CXX) $(ASANFLAGS) -o $@ $^

$(OBJDIR)/asan_%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(ASANFLAGS) -c $< -o $@

# --- Limpeza ---
clean:
	rm -rf $(OBJDIR) $(TARGET) $(DEBUG_TARGET) $(PROFILE_TARGET) $(ASAN_TARGET) gmon.out *.txt

# --- Regras de conveniência ---
rebuild: clean all