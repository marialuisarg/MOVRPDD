# Variáveis
CXX = g++

CXXFLAGS = -O3 -I./include -MMD -MP
DEBUGFLAGS = -g -O0 -I./include -MMD -MP
PROFILEFLAGS = -pg -g -I./include -MMD -MP
ASANFLAGS = -g -fsanitize=address -I./include -MMD -MP
UBSANFLAGS = -g -fsanitize=undefined -I./include -MMD -MP

SRCDIR = ./src
OBJDIR = ./obj

TARGET = MOVRPDD
DEBUG_TARGET = MOVRPDD-debug
PROFILE_TARGET = MOVRPDD-profile
ASAN_TARGET = MOVRPDD-asan
UBSAN_TARGET = MOVRPDD-ubsan

# Lista de arquivos fonte e objetos
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEBUG_OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/debug_%.o)
PROFILE_OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/profile_%.o)
ASAN_OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/asan_%.o)
UBSAN_OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/ubsan_%.o)

# Variável para os arquivos de dependência (.d)
DEPS = $(OBJECTS:.o=.d) $(DEBUG_OBJECTS:.o=.d) $(PROFILE_OBJECTS:.o=.d) $(ASAN_OBJECTS:.o=.d) $(UBSAN_OBJECTS:.o=.d)

# Regra padrão
all: $(TARGET)

# --- Regras para o executável normal ---
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regras para o executável de debug ---
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CXX) $(DEBUGFLAGS) -o $@ $^

$(OBJDIR)/debug_%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(DEBUGFLAGS) -c $< -o $@

# --- Regras para o executável de profiling ---
profile: $(PROFILE_TARGET)

$(PROFILE_TARGET): $(PROFILE_OBJECTS)
	$(CXX) $(PROFILEFLAGS) -o $@ $^ 

$(OBJDIR)/profile_%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(PROFILEFLAGS) -c $< -o $@

# --- Regras para o AddressSanitizer ---
asan: $(ASAN_TARGET)

$(ASAN_TARGET): $(ASAN_OBJECTS)
	$(CXX) $(ASANFLAGS) -o $@ $^

$(OBJDIR)/asan_%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(ASANFLAGS) -c $< -o $@

# --- Regras para o UndefinedBehaviorSanitizer ---
ubsan: $(UBSAN_TARGET)

$(UBSAN_TARGET): $(UBSAN_OBJECTS)
	$(CXX) $(UBSANFLAGS) -o $@ $^

$(OBJDIR)/ubsan_%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(UBSANFLAGS) -c $< -o $@

# --- Limpeza ---
clean:
	rm -rf $(OBJDIR) $(TARGET) $(DEBUG_TARGET) $(PROFILE_TARGET) $(ASAN_TARGET) $(UBSAN_TARGET) gmon.out *.txt

# --- Regras de conveniência ---
rebuild: clean all

# Inclui os arquivos de dependência gerados, se existirem
-include $(DEPS)