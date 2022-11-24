#Names of programs
MAIN    = $(firstword $(MAKECMDGOALS))
ifeq ($(MAIN),clean)
    MAIN =
endif

BIN     = ~/bin
BUILD   = ../build
RM      = /bin/rm -f
MV      = /bin/mv -f

LFLAGS  = -Wl,-rpath,/usr/bin/g++
LIBDIRS = $(LFLAGS) -L/usr/local/lib/ -L/usr/lib/boost/stage/lib/ -lstdc++fs
LIBS    = -lboost_program_options -lncurses -lpthread -lpython3.8

INC     = -I /usr/lib/boost/ -I /usr/include/python3.8/
CFLAGS  = -Wno-deprecated-declarations -fopenmp -std=c++17 -g
CC      = /usr/bin/g++ $(CFLAGS) $(INC) $(LIBS) 

#Output coloring
GREEN   = \033[1;32m
CYAN    = \033[36m
BLUE    = \033[1;34m
BRIGHT  = \033[1;37m
WHITE   = \033[0;m
MAGENTA = \033[35m
YELLOW  = \033[33m

#Source files
SRCS    = csim.c++          \
          cbody.c++         \
          simio.c++         \
          global.c++        \
          vel.c++           \
          vec.c++           \
          force.c++         \
          pos.c++
OBJS    = $(BUILD)/csim.o          \
          $(BUILD)/cbody.o         \
          $(BUILD)/pos.o           \
          $(BUILD)/vel.o           \
          $(BUILD)/vec.o           \
          $(BUILD)/force.o         \
          $(BUILD)/simio.o         \
          $(BUILD)/global.o        \
          $(BUILD)/integrator.o    \
          $(BUILD)/mercury.o       \
          $(BUILD)/miller.o        \
          $(BUILD)/bulirschStoer.o
          

#Builds
all:
	@printf "Specify build target\n"

$(BUILD)/%.o: %.c++
	@printf "[$(CYAN)Building$(WHITE)]   $(BRIGHT)$<$(WHITE) - $(MAGENTA)Object$(WHITE)\n"
	$(CC) -c -o $@ $<
	@printf "[$(GREEN) Built  $(WHITE)]   $(BRIGHT)$<$(WHITE) - $(MAGENTA)Object$(WHITE)\n"

$(MAIN): $(OBJS) $(BUILD)/$(MAIN).o
	@printf "[$(CYAN)Building$(WHITE)]   $(BRIGHT)$(MAIN).c++$(WHITE) - $(MAGENTA)Binary$(WHITE)\n"
	$(CC) $(OBJS) $(BUILD)/$(MAIN).o $(LIBDIRS) -o $(BIN)/$(MAIN) $(LIBS)
	@printf "[$(GREEN) Built  $(WHITE)]   $(BRIGHT)$(MAIN).c++$(WHITE) - $(MAGENTA)Binary$(WHITE)\n"

clean:
	$(RM) *.core $(BUILD)/*.o *.d *.stackdump
 
#Disable command echoing, reenabled with make verbose=1
ifndef verbose
.SILENT:
endif
