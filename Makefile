NAME      := webserv

CXX       := clang++
CXXFLAGS  := -Wall -Wextra -Werror -std=c++98

OBJDIR    := ./objs
DPSDIR    := ./dps

INCLUDE   := -I ./src/event -I ./src/exception -I ./src/socket -I ./src/event/mode -I ./src/request -I ./src/response -I ./src/extended_c -I ./src/config -I ./src/cgi

VPATH     := src: \
			src/cgi \
			src/event \
			src/event/mode \
			src/exception \
			src/extended_c \
			src/request \
			src/response \
			src/socket \
			src/config

CGI       := CGI.cpp \
			ReadCGI.cpp \
			WriteCGI.cpp

CONFIG    := Config.cpp \
			ConfigParser.cpp \
			ConfigValidator.cpp \
			LocationConfig.cpp \
			ServerConfig.cpp \
			Utils.cpp \

EVENT     := ReadFile.cpp \
			RecvRequest.cpp \
			SendResponse.cpp \
			SendError.cpp \
			AcceptConn.cpp \
			EventExecutor.cpp \
			EventRegister.cpp \

SOCKET    := ListeningSocket.cpp \

RESPONSE  := HTTPResponse.cpp \

REQUEST   := parse.cpp \
			HTTPRequest.cpp \
			HTTPParser.cpp \

EXCEPTION := SysError.cpp \

TESTSRCS  := $(CGI) \
			$(CONFIG) \
			$(EVENT) \
			$(SOCKET) \
			$(REQUEST) \
			$(RESPONSE) \
			$(EXCEPTION) \
# Delete.cpp

SRCS := main.cpp \
			$(TESTSRCS)

OBJS      := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.cpp=.o)))
TESTOBJS  := $(addprefix $(OBJDIR)/, $(notdir $(TESTSRCS:.cpp=.o)))
DPS       := $(addprefix $(DPSDIR)/, $(notdir $(SRCS:.o=.d)))

RM        := rm -rf

.PHONY: all
all: makedir $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

$(OBJDIR)/%.o: %.cpp
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
tidy:
	clang-tidy `find src -type f` -- $(INCLUDE)

.PHONY: tidy-fix
tidy-fix:
	clang-tidy `find src include -type f` --fix -- $(INCLUDE)

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
	$(testdir)/gtest.cpp $(gtestdir)/googletest-release-1.11.0/googletest/src/gtest_main.cc $(gtestdir)/gtest/gtest-all.cc $(TESTOBJS) \
	-g -fsanitize=address -fsanitize=undefined \
	-I$(gtestdir) $(INCLUDE) -lpthread -o tester

.PHONY: gtest_compile
gtest_compile:
	clang++ -std=c++11 \
	$(testdir)/gtest.cpp $(gtestdir)/googletest-release-1.11.0/googletest/src/gtest_main.cc $(gtestdir)/gtest/gtest-all.cc $(TESTOBJS) \
	-g -fsanitize=address -fsanitize=undefined \
	-I$(gtestdir) $(INCLUDE) -lpthread -o tester

.PHONY: gtest
gtest: $(gtest)
	$(test_compile)
	./tester

gtestlist:
	@./tester --gtest_list_tests
	@echo "\nRUN ./tester --gtest_filter=(TESTCASE).(TESTNAME)"

.PHONY: cleangtest
cleangtest:
	$(RM) $(gtestdir)
