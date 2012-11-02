#project dir setting
INCDIR = inc
SRCDIR = src
BINDIR = bin
LIBOUTPUTDIR = ./bin
GTEST_DIR = /usr/gtest

#DEBUG Output
DEBUGDIR = debug

#tools setting
CCTOOL 	= gcc
CXXTOOL = g++
ARTOOL 	= ar
MAKETOOL = make

#flags setting
DEBUGFLAGS = -O3 -g3 -Wall -D_DEBUG_LOGGING  -D_GNU_SOURCE
CCFLAGS = -c -fPIC
ARFLAGS = crv

LINKFLAGS = -lpthread 
CCFLAGS += $(DEBUGFLAGS) -I$(INCDIR) -I$(GTEST_DIR)/include
CXXFLAGS = $(CCFLAGS) -D__cplusplus
####################################################################################
# at almost time, you just need to change this part                           begin#
####################################################################################
#set objects
OBJECTS =  platform.o       \
       uti_file.o           \
	   uti_logging.o        \
	   uti_time.o		 	\
	   uti_thread.o			\
	   uti_common.o         \
	   uti_cstring.o		\
	   uti_rbtree.o         \
	   uti_timer.o          \
	   uti_errno.o          \
	   uti_mem_mgr.o        \
	   test_main.o

#library settings
LIBOBJECTS = platform.o     \
       uti_file.o           \
	   uti_logging.o        \
	   uti_time.o		 	\
	   uti_thread.o			\
	   uti_common.o         \
	   uti_cstring.o		\
	   uti_rbtree.o         \
	   uti_errno.o          \
	   uti_mem_mgr.o        \
	   uti_timer.o          	  

#dependency lib targets
DEPENDENCY_TARGETS	= 	
#static
ARTARGET = libplatform.a
#shared
SHAREDTARGET = libplatform.so
#test application target
TARGET = test_platfrom

####################################################################################
#                                                                               end#
####################################################################################
#making rules
all :  dependency $(TARGET)

$(TARGET) : $(OBJECTS)
	@echo creating target [$@]
	$(CCTOOL) -o $@ $^ $(LINKFLAGS)

%.o : $(SRCDIR)/%.cpp
	@echo compiling [$<]
	$(CXXTOOL) $(CXXFLAGS) $<

%.o : $(SRCDIR)/%.c
	@echo compiling [$<]
	$(CCTOOL) $(CCFLAGS) $<

test_main.o : test/test_main.c
	@echo compiling [$<]
	$(CCTOOL) $(CCFLAGS) $<

dependency : $(DEPENDENCY_TARGETS)
	@echo dependency checked over.


#this two rules just a example , for real usage, you should 
# add your own rule to create the library
static : $(LIBOBJECTS)
	@echo making static library with [$^]
	@if ! [ -d $(BINDIR) ] ; then mkdir $(BINDIR) ; fi
	$(ARTOOL) $(ARFLAGS) $(ARTARGET)  $^ 
	@echo copy $(ARTARGET) to $(LIBOUTPUTDIR)
	@if [ -d $(LIBOUTPUTDIR) ] ; then 	\
		cp $(ARTARGET) $(LIBOUTPUTDIR) \
	; fi

dynamic : $(LIBOBJECTS) 
	@echo "making shared library with $^ "
	@if ! [ -d $(BINDIR) ] ; then mkdir $(BINDIR) ; fi
	$(CXXTOOL) -shared -o  $(SHAREDTARGET) $^ $(LINKFLAGS)
	@echo copy $(SHAREDTARGET) to $(LIBOUTPUTDIR)
	@if [ -d $(LIBOUTPUTDIR) ] ; then 		\
		cp $(SHAREDTARGET) $(LIBOUTPUTDIR) 	\
	; fi


.PHONY : clean 
clean:
	@echo "clean all *.o pattern files..."
	@rm -f *.o $(TARGET) $(SHAREDTARGET) $(ARTARGET)


