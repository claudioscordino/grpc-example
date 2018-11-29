// See tutorial at https://grpc.io/docs/tutorials/basic/c.html

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
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

	private:
		std::unique_ptr<Master::Stub> stub_;
		std::shared_ptr<Channel> channel_;
};


int main ()
{
	std::string server_address ("localhost:50051");
	ClientNode client(server_address);

	VariableValue ret;
	VariableName name;
	name.set_name("varname1");
  	if (client.getVariable(&name, &ret))
		std::cout << "Client retrieved = " << ret.value() << std::endl;
	name.set_name("varname2");
	client.traceVariable(&name);

	return 0;
}
