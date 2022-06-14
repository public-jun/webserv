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

# SRCS := main.cpp \
# 			$(TESTSRCS)

# DPS    := $(addprefix $(DPSDIR)/, $(notdir $(SRCS:.o=.d)))
# OBJS      := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.cpp=.o)))
#
OBJS := $(patsubst $(SRCSDIR)%,$(OBJDIR)%,$(SRCS:.cpp=.o))
DPS   := $(patsubst $(SRCSDIR)%,$(DPSDIR)%,$(SRCS:.cpp=.d))


RM        := rm -rf

.PHONY: all
all: makedir $(NAME)

debug:
	@echo $(OBJS)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: %.cpp
	@echo $(@D)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -MMD -MP -MF $(DPSDIR)/$(notdir $(<:.cpp=.d)) -c $< -o $@

-include $(DPS)

.PHONY: makedir
makedir :
	@mkdir -p $(OBJDIR)
	@mkdir -p $(DPSDIR)

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(DPSDIR)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME) *.dSYM tester

.PHONY: re
re: fclean all

.PHONY: tidy
tidy: # Run clang-tidy
	clang-tidy `find src -type f` -- $(INCLUDE)

.PHONY: tidy-fix
tidy-fix: # Run clang-tidy --fix
	clang-tidy `find src include -type f` --fix -- $(INCLUDE)


################# google test ####################


gtest: # Create tester
	@$(MAKE) -C $(TESTDIR) gtest
	@mv $(TESTDIR)/tester ./

gtestclean:
	@$(MAKE) -C $(TESTDIR) clean

.PHONY: gtestlist
gtestlist:
	@./tester --gtest_list_tests
	@echo '\nRUN ./tester --gtest_filter="(TESTCASE).(TESTNAME)"'
