PATH_TO_SOURCES :=  ../../../src/tests/
program_CXX_SRCS := $(wildcard $(PATH_TO_SOURCES)*.cpp)
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}

tests:
	g++ -std=c++14 -W -Wall -g -lpthread -L/home/yurai/Downloads/boost_1_58_0/bin.v2/libs/system/build/gcc-4.9.2/release/link-static/threading-multi -lboost_system -I/home/yurai/Downloads/boost_1_58_0 -o tests ../../../src/tests/tests.cpp -lpthread -L/home/yurai/Downloads/boost_1_58_0/bin.v2/libs/system/build/gcc-4.9.2/release/link-static/threading-multi -lboost_system

clean:
	@- $(RM) tests
	@- $(RM) $(program_CXX_OBJS)

distclean: clean
