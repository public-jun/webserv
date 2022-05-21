NAMEMINI := mini
NAMESOCKET := socket
NAMEEVENT := event

MINIDIR := ./mini-echo
SOCKETDIR :=  ./echo-socket-class
EVENTDIR := ./echo-event-class

CXX       := clang++

RM := rm -rf

$(NAMEMINI):
	$(CXX) $(MINIDIR)/*.cpp -o $(NAMEMINI)

$(NAMESOCKET):
	$(CXX) $(SOCKETDIR)/*.cpp -o $(NAMESOCKET)

$(NAMEEVENT):
	$(CXX) $(EVENTDIR)/*.cpp -o $(NAMEEVENT)

.PHONY: fclean
fclean:
	$(RM) $(NAMEMINI) $(NAMESOCKET) $(NAMEEVENT)
