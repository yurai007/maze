PATH_TO_SOURCES :=  ../../../src/client/
PATH_TO_SOURCES2 :=  ../../../src/common/
CXXFLAGS += -std=c++14 -W -Wall -Ofast `pkg-config gtkmm-3.0 --cflags --libs` -lpthread -L/home/yurai/Downloads/boost_1_58_0/bin.v2/libs/system/build/gcc-4.9.2/release/link-static/threading-multi -lboost_system
LFLAGS += `pkg-config gtkmm-3.0 --cflags --libs`
program_NAME := maze_client
program_C_SRCS := $(wildcard $(PATH_TO_SOURCES2)*.c $(PATH_TO_SOURCES)*.c)
program_CXX_SRCS := $(wildcard $(PATH_TO_SOURCES2)*.cpp $(PATH_TO_SOURCES)*.cpp)
program_C_OBJS := ${program_C_SRCS:.c=.o}
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)
program_INCLUDE_DIRS := /home/yurai/Downloads/boost_1_58_0
program_LIBRARY_DIRS := /home/yurai/Downloads/boost_1_58_0/bin.v2/libs/system/build/gcc-4.9.2/release/link-static/threading-multi
program_LIBRARIES := boost_system

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: all clean distclean

maze_client: $(program_OBJS)
	$(LINK.cc) $(program_OBJS) -o $(program_NAME) -lpthread -L/home/yurai/Downloads/boost_1_58_0/bin.v2/libs/system/build/gcc-4.9.2/release/link-static/threading-multi -lboost_system `pkg-config gtkmm-3.0 --cflags --libs`

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

distclean: clean

#$(warning program_CXX_SRCS is $(program_CXX_SRCS))
#-L$$PWD/../../../../../lib/x86_64-linux-gnu/ -lpthread
