// See tutorial at https://grpc.io/docs/tutorials/basic/c.html

#define MAX_FILE_SIZE 102400

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "master.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using master::Master;
using master::VariableName;
using master::VariableValue;
using master::Chunk;
using master::FileInfo;
using master::Result;


class ClientNode {
	public:
		ClientNode(std::string server_address) {
			channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
			stub_ = Master::NewStub(channel_);
		}

 		bool getVariable(const VariableName* name, VariableValue* value) {
    			ClientContext context;
    			Status status = stub_->getVariable(&context, *name, value);
    			if (!status.ok()) {
      				std::cout << "Client: rpc failed. Is server running ?" << std::endl;
      				return false;
    			}
    			return true;
  		}

		// "server-side streaming"
		// The client receives a stream of data from the server.
		// Suitable for monitoring a variable on the server-side.
 		bool traceVariable(const VariableName* name) {
    			ClientContext context;
			VariableValue val;
			std::unique_ptr<ClientReader<VariableValue> > reader(
        				stub_->traceVariable(&context, *name));
    			while (reader->Read(&val)) {
				std::cout << "Client: received value " << val.value() << std::endl;
			}
    			Status status = reader->Finish();
			if (status.ok()) {
      				std::cout << "Client: tracing succeeded" << std::endl;
				return true;
    			} else {
      				std::cout << "Client: tracing failed" << std::endl;
				return false;
    			}
  		}

		// "client-side streaming"
		// The client sends a stream of data to the server.
		// Useful for sending big chunks of data (e.g. file upload)
		bool uploadFile (const std::string local_filename, const std::string remote_filename) {
			char buffer [MAX_FILE_SIZE];
			unsigned int read_size = 0, uploaded_size = 0;
			std::fstream file;
			std::cout << "Client: file transfer " << local_filename << " => " << 
				remote_filename << std::endl;
    			file = std::fstream(local_filename, std::ios::in | std::ios::binary);
			for (read_size = 0; read_size < MAX_FILE_SIZE; ++read_size) {
				file.read((char*) &(buffer[read_size]), sizeof(char));
				if (file.eof())
					break;
			}
			file.close();
			std::cout << "Client: read " << read_size << " bytes." << std::endl;



    			ClientContext context;
			FileInfo server_file;
			server_file.set_filename(remote_filename);
			Result reply;
    			Status status = stub_->setFileName(&context, server_file, &reply);
    			if (!status.ok()) {
      				std::cout << "Client: rpc failed. Is server running ?" << std::endl;
      				return false;
    			}

    			ClientContext context2;
			Chunk chunk;
			chunk.set_data(&buffer[0], read_size);
        		stub_->uploadFile(&context2, chunk, &reply);
		}

	private:
		std::unique_ptr<Master::Stub> stub_;
		std::shared_ptr<Channel> channel_;
};


int main ()
{
	std::string server_address ("localhost:50051");
	ClientNode client(server_address);

	client.uploadFile("in.pdf", "out.pdf");

	VariableValue ret;
	VariableName name;
	name.set_name("varname1");
  	if (client.getVariable(&name, &ret))
		std::cout << "Client retrieved = " << ret.value() << std::endl;
	name.set_name("varname2");
	client.traceVariable(&name);

	return 0;
}
