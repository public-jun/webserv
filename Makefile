NAME      := webserv

CXX       := clang++
CXXFLAGS  := -Wall -Wextra -Werror -std=c++98

INCDIR    :=    ./include
OBJDIR    :=    ./objs
DPSDIR    :=    ./dps

INCLUDE   := -I$(INCDIR)
VPATH     := src:

SRCS      := main.cpp \
			ServerSocket.cpp \
			SocketAddress.cpp \

OBJS      := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.cpp=.o)))
DPS       := $(addprefix $(DPSDIR)/, $(notdir $(SRCS:.o=.d)))

RM        := rm -rf

all: makedir $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS) $(LIBS)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -MMD -MP -MF $(DPSDIR)/$(notdir $(<:.cpp=.d)) -c $< -o $@

-include $(DPS)

makedir :
	mkdir -p $(OBJDIR)
	mkdir -p $(DPSDIR)

clean:
	rm -rf $(OBJDIR) $(DPSDIR)

fclean: clean
	$(RM) $(NAME) *.dSYM tester

re: fclean all

################# google test ####################

gtestdir =    ./google_test
gtest    =    $(gtestdir)/gtest $(gtestdir)/googletest-release-1.11.0
testdir  = ./test

$(gtest):
	mkdir -p $(dir ../google_test)
	curl -OL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
	tar -xvzf release-1.11.0.tar.gz googletest-release-1.11.0
	$(RM) -rf release-1.11.0.tar.gz
	python googletest-release-1.11.0/googletest/scripts/fuse_gtest_files.py $(gtestdir)
	mv googletest-release-1.11.0 $(gtestdir)

test_compile = clang++ -std=c++11 \
	$(testdir)/gtest.cpp $(gtestdir)/googletest-release-1.11.0/googletest/src/gtest_main.cc $(gtestdir)/gtest/gtest-all.cc \
	-g -fsanitize=address -fsanitize=undefined -fsanitize=leak \
	-I$(gtestdir) $(INCLUDE) -lpthread -o tester

.PHONY: gtest
gtest: $(gtest)
	$(test_compile)
	./tester
# ./tester # --gtest_filter=Vector.other

.PHONY: cleangtest
cleangtest:
	$(RM) $(gtestdir)
