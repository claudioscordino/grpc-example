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
		ClientNode(std::shared_ptr<Channel> channel)
      			: stub_(Master::NewStub(channel)) {}
 		bool getVariable(const VariableName* name, VariableValue* value) {
    			ClientContext context;
    			Status status = stub_->getVariable(&context, *name, value);
    			if (!status.ok()) {
      				std::cout << "rpc failed." << std::endl;
      				return false;
    			}
    			return true;
  		}


	private:
		std::unique_ptr<Master::Stub> stub_;
};


int main ()
{
  	ClientNode client(grpc::CreateChannel("localhost:50051",
                          	grpc::InsecureChannelCredentials()));

  	std::cout << "-------------- GetFeature --------------" << std::endl;
	VariableValue ret;
	VariableName name;
	name.set_name("varname");
  	client.getVariable(&name, &ret);
	std::cout << "Retrieved = " << ret.value() << std::endl;

	return 0;
}
