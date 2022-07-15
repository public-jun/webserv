################# google test ####################
NAME      := tester
LIBNAME   := libgtest.a
SRCSDIR   := ../../src
INCDIR    := $(shell find $(SRCSDIR) -maxdepth 2 -type d)
INCLUDE   := $(addprefix -I, $(INCDIR))

gtestdir  := ./google_testlib
gtest     := $(gtestdir)/gtest $(gtestdir)/googletest-release-1.11.0

CXX       := clang++
CXXFLAGS  := -std=c++11 -I$(gtestdir) $(INCLUDE)

OBJDIR    := ./obj

OBJS      := $(shell find ../../obj -name '*.o' -not -name 'main.o')

G_SRC :=  gtest.cc \
					$(gtestdir)/googletest-release-1.11.0/googletest/src/gtest_main.cc \
					$(gtestdir)/gtest/gtest-all.cc

G_OBJ := $(G_SRC:.cc=.o)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -MMD -MP -MF $(DPSDIR)/$(notdir $(<:.cpp=.d)) -c $< -o $@

$(gtest):
	mkdir -p $(dir ../google_test)
	curl -OL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
	tar -xvzf release-1.11.0.tar.gz googletest-release-1.11.0
	$(RM) -rf release-1.11.0.tar.gz
	python googletest-release-1.11.0/googletest/scripts/fuse_gtest_files.py $(gtestdir)
	mv googletest-release-1.11.0 $(gtestdir)

.PHONY: gtest
gtest: $(gtest) $(G_OBJ)
	$(CXX) $(CXXFLAGS) $(OBJS) $(G_OBJ) -o $(NAME)

.PHONY: clean
clean:
	$(RM) $(G_OBJ)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)
	rm -rf $(gtestdir)
