PATH_TO_SOURCES :=  ../../../src/tests/
CXXFLAGS += -std=c++14 -W -Wall -g -lpthread -lboost_system
program_NAME := tests
program_CXX_SRCS := $(wildcard $(PATH_TO_SOURCES)*.cpp)
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_CXX_OBJS)
program_LIBRARIES := boost_system

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: all clean distclean

tests: $(program_OBJS)
	$(LINK.cc) $(program_OBJS) -o $(program_NAME) -lpthread -lboost_system

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

distclean: clean
