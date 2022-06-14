NAME      := webserv

CXX       := clang++
CXXFLAGS  := -Wall -Wextra -Werror -std=c++98

SRCSDIR  := ./src
OBJDIR    := ./objs
DPSDIR    := ./dps
TESTDIR   := ./test

INCLUDE   := -I ./src/event \
						 -I ./src/event/mode \
						 -I ./src/exception \
						 -I ./src/socket \
						 -I ./src/request \
						 -I ./src/response \
						 -I ./src/extended_c \
						 -I ./src/config \
						 -I ./src/cgi \
						 -I ./src/uri

VPATH     := src: \
			src/cgi \
			src/event \
			src/event/mode \
			src/exception \
			src/extended_c \
			src/request \
			src/response \
			src/socket \
			src/config \
			src/uri

SRCS := $(shell find $(SRCSDIR) -type f -name '*.cpp')

OBJS := $(patsubst $(SRCSDIR)%,$(OBJDIR)%,$(SRCS:.cpp=.o))

RM        := rm -rf

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
	rm -rf $(OBJDIR) $(DPSDIR)

.PHONY: fclean
fclean: clean ## Remove object file, executable file
	$(RM) $(NAME) *.dSYM tester

.PHONY: re
re: fclean all ## Rebuild

.PHONY: tidy
tidy: ## Run clang-tidy
	clang-tidy `find src -type f` -- $(INCLUDE)

.PHONY: tidy-fix
tidy-fix: ## Run clang-tidy --fix
	clang-tidy `find src include -type f` --fix -- $(INCLUDE)

################# google test ####################

gtest: $(OBJS) ## Create tester
	@$(MAKE) -C $(TESTDIR) gtest
	@mv $(TESTDIR)/tester ./

gtestclean: ## Clean google test object file
	@$(MAKE) -C $(TESTDIR) clean

BLUE := \033[34m
RESET := \033[39m

.PHONY: gtestlist
gtestlist: gtest ## Show google test list
	@./tester --gtest_list_tests
	@printf '\n$(BLUE)RUN ./tester --gtest_filter="(TESTCASE).(TESTNAME)"$(RESET)\n'

PHONY: help
help: ## Display this help screen
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' "Makefile" | awk -F ' ##' '{printf "$(BLUE)%-20s$(RESET) %s\n", $$1, $$2}'
