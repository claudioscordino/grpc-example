
all:
	protoc --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` messages.proto
	protoc --cpp_out=. messages.proto

.PHONY: clean

clean:
	rm -fr *.pb.* *.grpc.*

