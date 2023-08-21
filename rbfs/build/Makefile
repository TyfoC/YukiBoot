COMPILER=g++
RESULT=rbfs.out

COMPILE_FLAGS=-Wall -Wextra -O2 -std=c++20 -I ../include -I ../../lib-common/rbfs
LINK_FLAGS=-Wall -Wextra -O2

SRCS=$(shell find ../source -name *.cpp)
OBJS=$(addsuffix .o, $(SRCS))

build:
	$(MAKE) $(OBJS)
	$(COMPILER) $(OBJS) $(LINK_FLAGS) -o $(RESULT)

clear:
	rm -f $(OBJS) $(RESULT)

%.cpp.o:	%.cpp
	$(COMPILER) -c $< $(COMPILE_FLAGS) -o $@