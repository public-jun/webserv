NAME      := webserv

CXX       := clang++
CXXFLAGS  := -Wall -Wextra -Werror -std=c++98

SRCSDIR   := ./src
OBJDIR    := ./obj
DPSDIR    := ./dps
INCDIR    := $(shell find $(SRCSDIR) -maxdepth 2 -type d)

INCLUDE := $(addprefix -I, $(INCDIR))
VPATH   := $(INCDIR)

SRCS := $(shell find $(SRCSDIR) -type f -name '*.cpp')
OBJS := $(patsubst $(SRCSDIR)%,$(OBJDIR)%,$(SRCS:.cpp=.o))

RM := rm -rf

.PHONY: all
all: $(NAME) ## Build

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(DPSDIR)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -MMD -MP -MF $(DPSDIR)/$(notdir $(<:.cpp=.d)) -c $< -o $@

-include $(DPS)

.PHONY: clean
clean: ## Remove object file
	$(RM) $(OBJDIR) $(DPSDIR)

.PHONY: fclean
fclean: clean ## Remove object file, executable file
	$(RM) $(NAME) *.dSYM tester

.PHONY: re
re: fclean all ## Rebuild

.PHONY: tidy
tidy: ## Run clang-tidy
	clang-tidy $(SRCS) -- $(INCLUDE)

.PHONY: tidy-fix
tidy-fix: ## Run clang-tidy --fix
	clang-tidy $(SRCS) --fix -- $(INCLUDE)

.PHONY: debug
debug: CXXFLAGS += -fsanitize=address -D WS_DEBUG=1
debug: re ## Debug mode rebuild


################# google test ####################

GOOGLETESTDIR := ./test/google_test

.PHONY: gtest
gtest: $(OBJS) ## Create tester
	@$(MAKE) -C $(GOOGLETESTDIR) gtest
	@mv $(GOOGLETESTDIR)/tester ./

.PHONY: gtestclean
gtestclean: ## Clean google test object file
	@$(MAKE) -C $(GOOGLETESTDIR) clean

BLUE  := \033[34m
RESET := \033[39m

.PHONY: gtestlist
gtestlist: gtest ## Show google test list
	@./tester --gtest_list_tests
	@printf '\n$(BLUE)RUN ./tester --gtest_filter="(TESTCASE).(TESTNAME)"$(RESET)\n'

PHONY: help
help: ## Display this help screen
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' "Makefile" | awk -F ' ##' '{printf "$(BLUE)%-20s$(RESET) %s\n", $$1, $$2}'

################# Go test ####################

GOTESTDIR := ./test/go_test

.PHONY: go_test
go_test:
	@$(MAKE) run -C $(GOTESTDIR)
