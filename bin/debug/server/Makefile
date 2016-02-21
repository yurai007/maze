PATH_TO_SOURCES :=  ../../../src/server/
PATH_TO_SOURCES2 :=  ../../../src/common/
CXXFLAGS += -std=c++14 -W -Wall -g `pkg-config gtkmm-3.0 --cflags --libs` -lpthread -lboost_system
LFLAGS += `pkg-config gtkmm-3.0 --cflags --libs`
program_NAME := maze_server
program_C_SRCS := $(wildcard $(PATH_TO_SOURCES2)*.c $(PATH_TO_SOURCES)*.c)
program_CXX_SRCS := $(wildcard $(PATH_TO_SOURCES2)*.cpp $(PATH_TO_SOURCES)*.cpp)
program_C_OBJS := ${program_C_SRCS:.c=.o}
program_CXX_OBJS := ${program_CXX_SRCS:.cpp=.o}
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)
program_LIBRARIES := boost_system

CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))

.PHONY: all clean distclean

maze_server: $(program_OBJS)
	$(LINK.cc) $(program_OBJS) -o $(program_NAME) -lpthread -lboost_system `pkg-config gtkmm-3.0 --cflags --libs`

clean:
	@- $(RM) $(program_NAME)
	@- $(RM) $(program_OBJS)

distclean: clean

#$(warning program_CXX_SRCS is $(program_CXX_SRCS))
#-L$$PWD/../../../../../lib/x86_64-linux-gnu/ -lpthread
