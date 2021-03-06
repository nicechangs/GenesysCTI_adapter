.SUFFIXES       : .cpp .o

CC              = gcc
CXX             = $(CC)

################################################
# IPRON Source Directory & Environment Setting #
#----------------------------------------------#
ifdef IPRBASE
IPRON_BASE      = $(IPRBASE)
else
IPRON_BASE      = /bridgetec/ipronv32
endif

IPRON_SRC       = $(IPRON_BASE)/src
CO_SRC          = $(IPRON_SRC)/CORE
IE_SRC          = $(IPRON_SRC)/IE
IC_SRC          = $(IPRON_SRC)/IC
ID_SRC          = $(IPRON_SRC)/ID
IR_SRC          = $(IPRON_SRC)/IR
IX_SRC          = $(IPRON_SRC)/IX
IL_SRC          = $(IPRON_SRC)/LIB

IE_BIN          = $(IPRON_BASE)/IE/bin
IC_BIN          = $(IPRON_BASE)/IC/bin
ID_BIN          = $(IPRON_BASE)/ID/bin
IR_BIN          = $(IPRON_BASE)/IR/bin
TL_BIN          = $(IPRON_BASE)/tools

CO_INC          = $(CO_SRC)/inc
CO_LIB          = $(CO_SRC)/lib

IL_INC          = $(IL_SRC)/inc
IL_LIB          = $(IL_SRC)/lib
IE_INC          = $(IE_SRC)/inc

IR_INC          = $(IR_SRC)/inc
IR_LIB          = $(IR_SRC)/lib


#####################################
# tinyxpath setting #
#-----------------------------------#
TINYXPATH_INC_PATH = $(IR_INC)/TinyXPath
TINYXPATH_LIB_PATH = $(IPRON_BASE)/ir/lib


#####################################
# Copy/Moving files when installing #
#-----------------------------------#
INC_FILES       = $(wildcard *.h)
INC_CPATH       = $(IR_INC)/GCTIDriver


LIB_FILES       = $(TARGET)
LIB_CPATH       = $(IPRON_BASE)/ir/lib
BIN_FILES       = # $(TARGET)
BIN_CPATH       = # $(TL_BIN)

###################################
# Include & Library Paths Setting #
#---------------------------------#
INC_PATHS       = -I$(CO_INC) -I$(CO_INC)/iCore -I$(IL_INC) -I$(IE_INC) -I$(IR_INC) -I$(TINYXPATH_INC_PATH)
LIB_PATHS       = -L$(CO_LIB) -L$(IL_LIB) -L$(TINYXPATH_LIB_PATH)

##########################################
# Define / Compile flags / Linking flags #
#----------------------------------------#
DEFINE          =
CFLAGS          = -g -Wall -fPIC -Wno-sign-compare -Wno-multichar
LFLAGS          = -lstdc++ -lTinyXPath -liCore 

############################
# Unit Testing for library #
#--------------------------#
UTSRC           = iTest.cpp
UTOBJ           = $(UTSRC:.cpp=.o)
UTBIN           = iTest

############################
# Main Sources and Objects #
# (Edit your TARGET name.) #
#--------------------------#
SRCS            = $(subst $(UTSRC),,$(wildcard *.cpp))
OBJS            = $(SRCS:.cpp=.o)
TARGET          = libGCTIDriver.so
INSTALLS        = $(TARGET)


##################################################
# Linking Part                                   #
# (Select linking option on your TARGET purpose) #
#------------------------------------------------#
all				: $(INSTALLS)
$(TARGET)		: $(OBJS)
#				@$(AR) -r $(TARGET) $(OBJS)
				@$(CC) -shared -o $(TARGET) $(OBJS) $(LFLAGS) $(LIB_PATHS)
#				@$(CC) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIB_PATHS)
				@echo "==============================================================="
				@echo "[35m$(TARGET) Compile Complete !![0m"


##########################
# DON'T edit below lines #
#------------------------#
$(UTBIN)		: $(OBJS) $(UTOBJ)
				@$(CC) -o $(UTBIN) $(OBJS) $(UTOBJ) $(LFLAGS) $(LIB_PATHS)
				@echo "==============================================================="
				@echo "[35m$(UTBIN) Compile Complete !![0m"
.cpp.o			:
				@echo "[$(TARGET)] Compiling [33m$<[0m ..."
				@$(CC) $(CFLAGS) -c $< -o $@ $(INC_PATHS) $(DEFINE) $(PKGDEF)
install			: $(INSTALLS)
ifdef BIN_FILES
ifdef BIN_CPATH
				@mkdir -p $(BIN_CPATH)
				@mv $(BIN_FILES) $(BIN_CPATH)/
				@echo "[$(TARGET)] [33mMove Binary Files($(BIN_FILES)) to $(BIN_CPATH)[0m ..."
endif
endif

ifdef INC_FILES
ifdef INC_CPATH
				@mkdir -p $(INC_CPATH)
				@cp $(INC_FILES) $(INC_CPATH)/
				@echo "[$(TARGET)] [33mCopy Include Files($(INC_FILES)) to $(INC_CPATH)[0m ..."
endif
endif

ifdef LIB_FILES
ifdef LIB_CPATH
				@mkdir -p $(LIB_CPATH)
				@cp $(LIB_FILES) $(LIB_CPATH)/
				@echo "[$(TARGET)] [33mCopy Library Files($(LIB_FILES)) to $(LIB_CPATH)[0m ..."
endif
endif
				@echo "==============================================================="
				@echo "[34m$(TARGET) Install Complete !![0m"
dep				:
				@gccmakedep $(INC_PATHS) $(SRCS)
				@echo "# Make Source Dependencies Success !!"
clean			:
				@echo "[$(TARGET)] Delete Object Files !!"
				@$(RM) $(OBJS) $(TARGET) $(UTOBJ) $(UTBIN)

