VPATH = ../src:../inc:../../common/inc:../../common/src
CFLAGS += $(patsubst %,-I%,$(subst :, ,$(VPATH)))
override CFLAGS += -lcunit
override CFLAGS += -lpthread
override CFLAGS += -g

SOURCE_OBJECTS = eLibrary_serverProcess.o eLibrary_serverBrief.o eLibrary_serverHandler.o eLibrary_serverCommon.o
TEST_SRV_OBJECTS = eLibrary_server.o test_server.o common.o
TEST_CLT_OBJECTS = eLibrary.o test_client.o common.o
TEST_UNI_OBJECTS = test_eLibrary.o test_serverHandler.o common.o

all: eLibrary_testServer eLibrary_testClient eLibrary_testUnit

eLibrary_testServer: $(SOURCE_OBJECTS) $(TEST_SRV_OBJECTS)
	cc $(CFLAGS) $^ -o $@ -W1,-rpath/usr/local/lib
eLibrary_testClient: $(SOURCE_OBJECTS) $(TEST_CLT_OBJECTS)
	cc $(CFLAGS) $^ -o $@ -W1,-rpath/usr/local/lib
eLibrary_testUnit: $(SOURCE_OBJECTS) $(TEST_UNI_OBJECTS)
	cc $(CFLAGS) $^ -o $@ -W1,-rpath/usr/local/lib

$(SOURCE_OBJECTS):

$(TEST_UNI_OBJECTS): 
$(TEST_SRV_OBJECTS): 
$(TEST_CLT_OBJECTS): 

PHONY: clean configure
clean:
	-rm *.o
	-rm *.xml
	-rm log.txt
	-rm eLibrary_testUnit	
	
configure:
	echo Chatty configured.
	
