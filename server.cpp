// See tutorial at https://grpc.io/docs/tutorials/basic/c.html

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


class ServerNode final : public Master::Service {
	public:
		Status getVariable(ServerContext* context, const VariableName* var,
                  		VariableValue* val) override {
			std::cout << "Server: request to read variable: " << var->name() << std::endl;
			val->set_value(42);
  			return Status::OK;
		}

		Status traceVariable(ServerContext* context, const VariableName* var,
                    		ServerWriter<VariableValue>* writer) override {
			std::cout << "Server: request to trace variable: " << var->name() << std::endl;
  			for (int i = 0; i < 100; ++i) {
				VariableValue var;
				var.set_value(i);
      				writer->Write(var);
    			}
  			return Status::OK;
		}
};


int main ()
{
	std::string server_address("0.0.0.0:50051");
  	ServerNode service;
  	ServerBuilder builder;
  	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  	builder.RegisterService(&service);
  	std::unique_ptr<Server> server(builder.BuildAndStart());
  	std::cout << "Server: listening on " << server_address << std::endl;
  	server->Wait();
	return 0;
}
