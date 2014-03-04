TARGETS = client scheduler
CC = gcc
INCS=-I. \
	-I../../ \
	-I../../ZHT \
	-I../../ZHT/src
CCFLAGS = -g -I${USER_INCLUDE} -I../../ -L${USER_LIB} -L../../ZHT/src
#CCFLAGS = -g -I${USER_INCLUDE} -L${USER_LIB} -DPF_INET -DBIG_MSG -DTHREADED_SERVE -DSOCKET_CACHE -DSCCB

LIBFLAGS = -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotobuf-c

all:	$(TARGETS)

client: client.o client_stub.o util.o config.o matrix_epoll_server.o metazht.pb.o
	$(CC) $(CCFLAGS) -o $@ $^ -PRINT_OUT $(LIBFLAGS)

scheduler: scheduler.o scheduler_stub.o util.o config.o matrix_epoll_server.o metazht.pb.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)
	
%.o: %.cpp
	$(CC) $(CCFLAGS) -c $^ $(LIBFLAGS) $(INCS)
	
%.o: %.cc
	$(CC) $(CCFLAGS) -c $^ $(LIBFLAGS) $(INCS)

util.o: util.cpp
config.o: config.cpp
matrix_epoll_server.o: matrix_epoll_server.cpp
metazht.pb.o: metazht.pb.cc

client_stub.o: client_stub.cpp
client.o: client.cpp

scheduler_stub.o: scheduler_stub.cpp
scheduler.o: scheduler.cpp

.PHONY:	clean

clean:	
	rm *.o
	rm $(TARGETS)