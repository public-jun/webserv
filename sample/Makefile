NAMEMINI := mini
NAMESOCKET := socket

MINIDIR := ./mini-echo
SOCKETDIR :=  ./echo-socket-class

CXX       := clang++

RM := rm -rf

$(NAMEMINI):
	$(CXX) $(MINIDIR)/*.cpp -o $(NAMEMINI)

$(NAMESOCKET):
	$(CXX) $(SOCKETDIR)/*.cpp -o $(NAMESOCKET)

.PHONY: fclean
fclean:
	$(RM) $(NAMEMINI) $(NAMESOCKET)
