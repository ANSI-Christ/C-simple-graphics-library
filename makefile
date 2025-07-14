
########################### VARIABLES ############################

TARGET      = test
COMPILER    = gcc
C_RELEASE   = -c -O3 -Wall
C_DEBUG     = -ggdb -c -Wall
C_FLAGS     = $(C_RELEASE)
#C_FLAGS     = $(C_DEBUG)
LD_FLAGS    =
SOURCES_C   =
SOURCES_M   =
OBJECTS     =
OBJECTS_ADD =

############################ FOLDERS #############################



##################################################################

C_FLAGS +=

##################################################################

LD_FLAGS += -lc -lm
LD_FLAGS += -lX11
#LD_FLAGS += -framework Cocoa

##################################################################

SOURCES_C += main.c
SOURCES_C += sgl/sgl.c

#SOURCES_M += sgl/sgw/sg_cocoa.m

##################################################################

OBJECTS += $(SOURCES_C:.c=.o)
OBJECTS += $(SOURCES_M:.m=.o)

##################################################################

all: $(SOURCES_C) $(SOURCES_M)  $(TARGET)

##################################################################

$(TARGET): $(OBJECTS) $(OBJECTS_ADD)
	$(COMPILER) $(OBJECTS) $(OBJECTS_ADD) -o $(TARGET) $(LD_FLAGS)

.c.o:
	$(COMPILER) $(C_FLAGS) $< -o $@

.m.o:
	$(COMPILER) $(C_FLAGS) $< -o $@

##################################################################

rmo:
	rm $(OBJECTS)

clean:
	rm $(OBJECTS) $(TARGET)
