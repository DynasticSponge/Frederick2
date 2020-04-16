# Frederick2

Frederick2 is a C++ HTTP Server library for Linux.

## Build

```bash
make static
```
The above will compile the source and generate the libfrederick2.a static library in the output directory

## Installation

Copy the 'headers' folder and libfrederick2.a library file to the appropriate locations with your project's folder structure and update your complier/linker appropriately

## Usage

```c++
#include <iostream>
#include "frederick2/frederick2_namespace.hpp"
#include "frederick2/http_request.hpp"
#include "frederick2/http_response.hpp"
#include "frederick2/server.hpp"
#include "frederick2/server_enum.hpp"
#include "frederick2/server_resource"

namespace enums = frederick2::httpEnums;
namespace packet = frederick2::httpPacket;
namespace server = frederick2::httpServer;

void buildResourceTree(server::resource* treeRoot)
{
    // build a handler function
    httpHandler rootGetHandler{[](packet::httpRequest* request, packet::httpResponse* response){
        std::string returnContent{"<!doctype html><html><head></head><body>"};
        returnContent.append("<h1>Hello World!</h1>");
        returnContent.append("<h3>You've executed a GET request against the root of 192.168.1.64</h3>");
        returnContent.append("</body></html>");
        response->setContent(returnContent);
        response->addHeader("Content-Type", "text/html");
        response->setStatus(enums::httpStatus::OK);
    }};

    // add the handler function to the resource for a specific HTTP method
    // in this case we're adding the handler for GET requests against the root resource
    // (ie http://127.0.0.1/ )
    treeRoot->addHandler(enums::httpMethod::GET, rootGetHandler);
    
    // create a new resource branch from the rootResource
    server::resource* rootBranchAction{treeRoot->addChild("actions", enums::resourceType::STATIC)};

    // build a new handler function    
    httpHandler actionPostHandler{[](packet::httpRequest* request, packet::httpResponse* response){
        std::string returnContent{"<!doctype html><html><head></head><body>"};
        returnContent.append("<h1>Hello World!</h1>");
        returnContent.append("<h3>You've executed a POST request against /action of 192.168.1.64</h3>");
        returnContent.append("</body></html>");
        response->setContent(returnContent);
        response->addHeader("Content-Type", "text/html");
        response->setStatus(enums::httpStatus::OK);
    }};

    // add the handler function to the resource for a specific HTTP method
    // in this case we're adding the handler for POST requests against the 'actions'
    // (ie http://127.0.0.1/actions )
    rootBranchAction->addHandler(enums::httpMethod::POST, actionPostHandler);
    
    // build a new handler function    
    httpHandler actionPutHandler{[](packet::httpRequest* request, packet::httpResponse* response){
        std::string returnContent{"<!doctype html><html><head></head><body>"};
        returnContent.append("<h1>Hello World!</h1>");
        returnContent.append("<h3>You've executed a PUT request against /action of 192.168.1.64</h3>");
        returnContent.append("</body></html>");
        response->setContent(returnContent);
        response->addHeader("Content-Type", "text/html");
        response->setStatus(enums::httpStatus::OK);
    }};
    
    // add the handler function to the resource for a specific HTTP method
    // in this case we're adding the handler for PUT requests against the 'actions'
    // (ie http://127.0.0.1/actions )
    rootBranchAction->addHandler(enums::httpMethod::PUT, actionPutHandler);

    // at this point the server will respond to GET requests at /
    // the server will also respond to POST and PUT requests at /actions
    return;
}

int main(int argc, char* argv[])
{
    // Instantiate the server
    server::httpServer *httpServer{new server::httpServer()};
    // Retrieve the root resource node
    server::resource *treeRoot{httpServer->getResourceTree()};
    // Build the resource tree
    buildResourceTree(treeRoot);
    // Set the IP address to which to bind the listen socket
    httpServer->setBindAddress("127.0.0.1");
    // Set the Port to which to bind the listen socket
    httpServer->setBindPort(8080);
    // Set the size of the TCP listen queue
    httpServer->setListenQueue(5);
    // Start the server
    if(!httpServer->start())
    {
        std::cout << "Could not start frederick2." << std::endl;
        std::cout << "Make sure Address, Port, and ListenQueue are set before calling start()" << std::endl;
    }

    // Do something else till whenever
    while(true)
    {
        continue;
    }

    // Stop the server
    httpServer->stop();
    delete treeRoot;
    delete httpServer;
    return(0);
}
```

## Contributing

This is a hobby project for me to have fun learning and tweaking.  I'm more than happy to share the results with whomever wants to make use of it.  If you are interested in contributing however, please open an issue and we can discuss what you would like to do.  

Thanks

## License
[MIT](https://choosealicense.com/licenses/mit/)