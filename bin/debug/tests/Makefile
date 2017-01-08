PATH_TO_SOURCES :=  ../../../src/tests/
CXXFLAGS += -std=c++14 -W -Wall -g -fconcepts -fsanitize=address -fsanitize-recover=address -fsanitize=undefined -fsanitize=vptr
CXXFLAGS += -fstack-protector -Wshadow -Wformat-security
program_NAME := tests
program_CXX_SRCS := $(wildcard $(PATH_TO_SOURCES)*.cpp)
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_CXX_OBJS)
program_LIBRARIES := boost_system

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))
LDFLAGS += -lpthread -lboost_system

.PHONY: all clean distclean

tests: $(program_OBJS)
	$(LINK.cc) $(program_OBJS) -o $(program_NAME) $(LDFLAGS)

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

distclean: clean
