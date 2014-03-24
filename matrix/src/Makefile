TARGETS = client scheduler
CC = gcc
INCS=-I. \
	-I../../ \
	-I../../ZHT \
	-I../../ZHT/src
CCFLAGS = -g -I${USER_INCLUDE} -L${USER_LIB} -DPRINT_OUT -DDATA_CACHE
#CCFLAGS = -g -I${USER_INCLUDE} -L${USER_LIB} -DPF_INET -DBIG_MSG -DTHREADED_SERVE -DSOCKET_CACHE -DSCCB

LIBFLAGS = -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotobuf-c -L../../ZHT/src -lzht

all:	$(TARGETS)

client: client.o client_stub.o config.o util.o metazht.pb.o metamatrix.pb.o metatask.pb.o ../../ZHT/src/cpp_zhtclient.o matrix_tcp_proxy_stub.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

scheduler: scheduler.o scheduler_stub.o config.o util.o matrix_epoll_server.o metazht.pb.o metamatrix.pb.o metatask.pb.o matrix_tcp_proxy_stub.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)
	
%.o: %.cpp
	$(CC) $(CCFLAGS) -c $^ $(LIBFLAGS) $(INCS)
	
%.o: %.cc
	$(CC) $(CCFLAGS) -c $^ $(LIBFLAGS) $(INCS)

config.o: config.cpp
util.o: util.cpp
matrix_epoll_server.o: matrix_epoll_server.cpp
metazht.pb.o: metazht.pb.cc
metamatrix.pb.o: metamatrix.pb.cc
metatask.pb.o: metatask.pb.cc
matrix_tcp_proxy_stub.o: matrix_tcp_proxy_stub.cpp

client_stub.o: client_stub.cpp
client.o: client.cpp

scheduler_stub.o: scheduler_stub.cpp
scheduler.o: scheduler.cpp

.PHONY:	clean

clean:	
	rm -rf *.o
	rm -rf $(TARGETS)