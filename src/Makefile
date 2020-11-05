#Names of programs
MAIN    = $(firstword $(MAKECMDGOALS))
ifeq ($(MAIN),clean)
    MAIN =
endif

ABS		= .
BIN		= .
BUILD	= $(ABS)/../build-wsl
OPT		= opt_
RM      = /bin/rm -f
MV		= /bin/mv -f

LFLAGS	= -Wl,-rpath,/usr/bin/g++
LIBDIRS	= $(LFLAGS) -L/usr/local/lib/ -L/usr/lib/boost/stage/lib/ -lstdc++fs
LIBS	= -lboost_program_options -lncurses -lpthread -lpython3.8

INC		= -I /usr/lib/boost/ -I /usr/include/python3.8/
CFLAGS	= -Wno-deprecated-declarations -fopenmp -std=c++17 -g
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
SRCS	= $(ABS)/$(OPT)csim.c++			\
		  $(ABS)/$(OPT)cbody.c++		\
		  $(ABS)/$(OPT)simio.c++		\
		  $(ABS)/$(OPT)global.c++		\
		  $(ABS)/$(OPT)vel.c++			\
		  $(ABS)/$(OPT)vec.c++			\
		  $(ABS)/$(OPT)force.c++		\
		  $(ABS)/$(OPT)pos.c++
OBJS	= $(BUILD)/$(OPT)csim.o			\
		  $(BUILD)/$(OPT)cbody.o		\
		  $(BUILD)/$(OPT)pos.o			\
		  $(BUILD)/$(OPT)vel.o			\
		  $(BUILD)/$(OPT)vec.o			\
		  $(BUILD)/$(OPT)force.o		\
		  $(BUILD)/$(OPT)simio.o		\
		  $(BUILD)/$(OPT)global.o		\
		  $(BUILD)/$(OPT)integrator.o	\
		  $(BUILD)/$(OPT)mercury.o      \
		  $(BUILD)/$(OPT)miller.o		\
		  $(BUILD)/$(OPT)bulirschStoer.o
		  

#Builds
all:
	@printf "Specify build target\n"

$(BUILD)/%.o: %.c++
	@printf "[$(CYAN)Building$(WHITE)]   $(BRIGHT)$<$(WHITE) - $(MAGENTA)Object$(WHITE)\n"
	cd $(ABS); $(CC) -c -o $@ $<
	@printf "[$(GREEN) Built  $(WHITE)]   $(BRIGHT)$<$(WHITE) - $(MAGENTA)Object$(WHITE)\n"

$(MAIN): $(OBJS) $(BUILD)/$(OPT)$(MAIN).o
	@printf "[$(CYAN)Building$(WHITE)]   $(BRIGHT)$(OPT)$(MAIN).c++$(WHITE) - $(MAGENTA)Binary$(WHITE)\n"
	cd $(ABS); $(CC) $(OBJS) $(BUILD)/$(OPT)$(MAIN).o $(LIBDIRS) -o $(BIN)/$(OPT)$(MAIN) $(LIBS)
	@printf "[$(GREEN) Built  $(WHITE)]   $(BRIGHT)$(OPT)$(MAIN).c++$(WHITE) - $(MAGENTA)Binary$(WHITE)\n"

clean:
	$(RM) *.core $(BUILD)/*.o *.d *.stackdump

#Disable command echoing, reenabled with make verbose=1
ifndef verbose
.SILENT:
endif
