#Name of program
MAIN    = bulirsch-stoer

ABS		= .
BIN		= ./bin
BUILD	= $(ABS)/build
RM      = /bin/rm -f
MV		= /bin/mv -f

LFLAGS	= -Wl,-rpath,/bin/g++.exe
LIBDIRS	= $(LFLAGS) -L/usr/local/lib/ -L/usr/lib/boost/stage/lib/ -lstdc++fs
LIBS	= -lboost_program_options

INC		= -I /usr/lib/boost/
CFLAGS	= -Wno-deprecated-declarations -fopenmp -std=c++17
CC      = /bin/g++.exe $(CFLAGS) $(INC) $(LIBS) 

#Output coloring
GREEN   = \033[1;32m
CYAN    = \033[36m
BLUE    = \033[1;34m
BRIGHT  = \033[1;37m
WHITE   = \033[0;m
MAGENTA = \033[35m
YELLOW  = \033[33m

#Source files
SRCS	= $(ABS)/$(MAIN).c++	\
		  $(ABS)/csim.c++		\
		  $(ABS)/cbody.c++		\
		  $(ABS)/hash.c++		\
		  $(ABS)/simio.c++		\
		  $(ABS)/pos.c++
OBJS	= $(BUILD)/$(MAIN).o	\
		  $(BUILD)/csim.o		\
		  $(BUILD)/cbody.o		\
		  $(BUILD)/pos.o		\
		  $(BUILD)/simio.o		\
		  $(BUILD)/hash.o
		  

#Builds
all:
	@printf "[      $(YELLOW)Building $(MAIN)$(WHITE)       ]\n"
	@printf "[$(BLUE)Building$(WHITE)] $(BRIGHT)$(MAIN)$(WHITE) - $(MAGENTA)Program$(WHITE)\n"
	make build
	@printf "[$(GREEN) Built  $(WHITE)] $(BRIGHT)$(MAIN)$(WHITE) - $(MAGENTA)Program$(WHITE)\n"
	@printf "[        $(GREEN)Build Complete$(WHITE)        ]\n"

$(BUILD)/%.o: %.c++
	@printf "[$(CYAN)Building$(WHITE)]   $(BRIGHT)$<$(WHITE) - $(MAGENTA)Object$(WHITE)\n"
	cd $(ABS); $(CC) -c -o $@ $<
	@printf "[$(GREEN) Built  $(WHITE)]   $(BRIGHT)$<$(WHITE) - $(MAGENTA)Object$(WHITE)\n"

build: $(OBJS)
	@printf "[$(CYAN)Building$(WHITE)]   $(BRIGHT)$(MAIN).c++$(WHITE) - $(MAGENTA)Binary$(WHITE)\n"
	cd $(ABS); $(CC) $(OBJS) $(LIBDIRS) -o $(BIN)/$(MAIN).exe $(LIBS)
	@printf "[$(GREEN) Built  $(WHITE)]   $(BRIGHT)$(MAIN).c++$(WHITE) - $(MAGENTA)Binary$(WHITE)\n"


clean:
	$(RM) *.core $(BUILD)/*.o *.d *.stackdump

#Disable command echoing, reenabled with make verbose=1
ifndef verbose
.SILENT:
endif
