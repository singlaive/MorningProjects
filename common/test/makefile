
COMMON_OBJS = common.o test.o
VPATH = ../src:../inc
override CFLAGS += $(patsubst %,-I%,$(subst :, ,$(VPATH)))
CFLAGS+=-lcunit
all: test_common

test_common: $(COMMON_OBJS)
	cc $^ -o $@ -lcunit -W1,-rpath/usr/local/lib
#	cc -o test_common $(COMMON_OBJS) -lcunit -W1,-rpath/usr/local/lib
	
#common.o: common.h
#	cc $(CFLAGS) -c $^ -o $@

#test.o: test.c 
#	cc $(CFLAGS) -c $^ -o $@

#$(COMMON_OBJS): ../inc/common.h

clean:
	rm *.o test_common
