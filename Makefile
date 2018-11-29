## See https://github.com/grpc/grpc/blob/v1.16.0/examples/cpp/route_guide/Makefile

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CPPFLAGS += -Wall -Wextra
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

.PHONY: clean proto system-check all

all: proto server

proto:
	$(PROTOC) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) master.proto
	$(PROTOC) --cpp_out=. master.proto

server:
	$(CXX) -Wall -Wextra server.cpp $(LDFLAGS) -o server


clean:
	rm -fr *.pb.* *.grpc.*

