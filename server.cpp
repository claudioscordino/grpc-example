#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>


#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include "master.grpc.pb.h"

// See tutorial at https://grpc.io/docs/tutorials/basic/c.html

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using master::Master;
using master::VariableName;
using master::VariableValue;


class ImechServer final : public Master::Service {
	public:
		explicit ImechServer() {}
		~ImechServer(){}
		explicit ImechServer (std::string name) {
  		}
		Status getVariable(ServerContext* context, const VariableName* var,
                  		VariableValue* val) override {
			val->set_value(42);
  			return Status::OK;
		}
	private:
};


int main ()
{
	std::string server_address("0.0.0.0:50051");
  	ImechServer service("pippo");
  	ServerBuilder builder;
  	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  	builder.RegisterService(&service);
  	std::unique_ptr<Server> server(builder.BuildAndStart());
  	std::cout << "Server listening on " << server_address << std::endl;
  	server->Wait();
	return 0;
}
