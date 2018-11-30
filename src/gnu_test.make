#Name of program
MAIN    = test

ABS		= .
BIN		= .
BUILD	= $(ABS)/../build
OPT		= 
RM      = /bin/rm -f
MV		= /bin/mv -f

LFLAGS	= -Wl,-rpath,/usr/local/bin/g++.exe
LIBDIRS	= $(LFLAGS) -L/usr/local/lib/ -L/usr/lib/boost/stage/lib/ -lstdc++fs
LIBS	= -lboost_program_options -lncurses

INC		= -I /usr/lib/boost/
CFLAGS	= -Wno-deprecated-declarations -fopenmp -std=c++17 -O3
CC      = /usr/local/bin/g++.exe $(CFLAGS) $(INC) $(LIBS) 

#Output coloring
GREEN   = \033[1;32m
CYAN    = \033[36m
BLUE    = \033[1;34m
BRIGHT  = \033[1;37m
WHITE   = \033[0;m
MAGENTA = \033[35m
YELLOW  = \033[33m

#Source files
SRCS	= $(ABS)/$(OPT)$(MAIN).c++	\
		  $(ABS)/$(OPT)csim.c++		\
		  $(ABS)/$(OPT)cbody.c++	\
		  $(ABS)/$(OPT)simio.c++	\
		  $(ABS)/$(OPT)global.c++	\
		  $(ABS)/$(OPT)vel.c++		\
		  $(ABS)/$(OPT)vec.c++		\
		  $(ABS)/$(OPT)rpos.c++		\
		  $(ABS)/$(OPT)force.c++	\
		  $(ABS)/$(OPT)pos.c++
OBJS	= $(BUILD)/$(OPT)$(MAIN).o	\
		  $(BUILD)/$(OPT)csim.o		\
		  $(BUILD)/$(OPT)cbody.o	\
		  $(BUILD)/$(OPT)pos.o		\
		  $(BUILD)/$(OPT)vel.o		\
		  $(BUILD)/$(OPT)vec.o		\
		  $(BUILD)/$(OPT)rpos.o		\
		  $(BUILD)/$(OPT)force.o	\
		  $(BUILD)/$(OPT)simio.o	\
		  $(BUILD)/$(OPT)global.o
		  

#Builds
all:
	@printf "[      $(YELLOW)Building $(OPT)$(MAIN)$(WHITE)       ]\n"
	@printf "[$(BLUE)Building$(WHITE)] $(BRIGHT)$(OPT)$(MAIN)$(WHITE) - $(MAGENTA)Program$(WHITE)\n"
	gmake -f gnu_$(OPT)test.gmake build
	@printf "[$(GREEN) Built  $(WHITE)] $(BRIGHT)$(OPT)$(MAIN)$(WHITE) - $(MAGENTA)Program$(WHITE)\n"
	@printf "[        $(GREEN)Build Complete$(WHITE)        ]\n"

$(BUILD)/%.o: %.c++
	@printf "[$(CYAN)Building$(WHITE)]   $(BRIGHT)$<$(WHITE) - $(MAGENTA)Object$(WHITE)\n"
	cd $(ABS); $(CC) -c -o $@ $<
	@printf "[$(GREEN) Built  $(WHITE)]   $(BRIGHT)$<$(WHITE) - $(MAGENTA)Object$(WHITE)\n"

build: $(OBJS)
	@printf "[$(CYAN)Building$(WHITE)]   $(BRIGHT)$(OPT)$(MAIN).c++$(WHITE) - $(MAGENTA)Binary$(WHITE)\n"
	cd $(ABS); $(CC) $(OBJS) $(LIBDIRS) -o $(BIN)/$(OPT)$(MAIN) $(LIBS)
	@printf "[$(GREEN) Built  $(WHITE)]   $(BRIGHT)$(OPT)$(MAIN).c++$(WHITE) - $(MAGENTA)Binary$(WHITE)\n"


clean:
	$(RM) *.core $(BUILD)/*.o *.d *.stackdump

#Disable command echoing, reenabled with gmake verbose=1
ifndef verbose
.SILENT:
endif
