# frederick2
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/ad520bdcf77e402ca58ca18315d0b8c7)](https://www.codacy.com/manual/joseph.adomatis/frederick2?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=DynasticSponge/frederick2&amp;utm_campaign=Badge_Grade)

frederick2 is a C++ HTTP server library for linux.

## Stage/Release

frederick2 is still in an alpha / pre-release stage of development

## Build

Modify the makefile to address your own folder structure or ensure that "object" and "output" folders exist as peers of the "source" and "headers" folders.

```bash
make static
```
The above will compile the source and generate the libfrederick2.a static library in the "output" folder

## Installation

Copy the 'headers' folder and libfrederick2.a library file to the appropriate locations within your project's folder structure and update your compiler/linker appropriately.

## Basic Example

```c++
#include <iostream>
#include "frederick2/frederick2_namespace.hpp"
#include "frederick2/http_request.hpp"
#include "frederick2/http_response.hpp"
#include "frederick2/server.hpp"
#include "frederick2/server_enum.hpp"
#include "frederick2/server_resource.hpp"

namespace enums = frederick2::httpEnums;
namespace packet = frederick2::httpPacket;
namespace server = frederick2::httpServer;

void buildResourceTree(server::resource* treeRoot)
{
    // build a handler function
    httpHandler rootGetHandler{[](packet::httpRequest* request, packet::httpResponse* response){
        std::string returnContent{"<!doctype html><html><head></head><body>"};
        returnContent.append("<h1>Hello World!</h1>");
        returnContent.append("<h3>You've executed a GET request against the root of 127.0.0.1</h3>");
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
    httpHandler actionsPostHandler{[](packet::httpRequest* request, packet::httpResponse* response){
        std::string returnContent{"<!doctype html><html><head></head><body>"};
        returnContent.append("<h1>Hello World!</h1>");
        returnContent.append("<h3>You've executed a POST request against /action of 127.0.0.1</h3>");
        returnContent.append("</body></html>");
        response->setContent(returnContent);
        response->addHeader("Content-Type", "text/html");
        response->setStatus(enums::httpStatus::OK);
    }};

    // add the handler function to the resource for a specific HTTP method
    // in this case we're adding the handler for POST requests against the 'actions'
    // (ie http://127.0.0.1/actions )
    rootBranchAction->addHandler(enums::httpMethod::POST, actionsPostHandler);
    
    // build a new handler function    
    httpHandler actionsPutHandler{[](packet::httpRequest* request, packet::httpResponse* response){
        std::string returnContent{"<!doctype html><html><head></head><body>"};
        returnContent.append("<h1>Hello World!</h1>");
        returnContent.append("<h3>You've executed a PUT request against /action of 127.0.0.1</h3>");
        returnContent.append("</body></html>");
        response->setContent(returnContent);
        response->addHeader("Content-Type", "text/html");
        response->setStatus(enums::httpStatus::OK);
    }};
    
    // add the handler function to the resource for a specific HTTP method
    // in this case we're adding the handler for PUT requests against the 'actions'
    // (ie http://127.0.0.1/actions )
    rootBranchAction->addHandler(enums::httpMethod::PUT, actionsPutHandler);

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
    httpServer->setBindPort(8443);
    // Set the size of the TCP listen queue
    httpServer->setListenQueue(5);
    
    // Set flag to use ssl encryption
    httpServer->setUseSSL(true);
    // Provide full path to private key file
    // start() validates it can open file
    httpServer->setSSLPrivateKey("/var/frederick2/ssl/serverkey.pem");
    // Provide full path to public cert file
    // start() validates it can open file
    httpServer->setSSLPublicCert("/var/frederick2/ssl/servercert.pem");
    
    // Start the server
    if(!httpServer->start())
    {
        std::cout << "Could not start frederick2." << std::endl;
        std::cout << "Make sure Address, Port, and ListenQueue are set before calling start()" << std::endl;
        std::cout << "If SSL, set paths to PEM formatted Key and Cert before calling start()" << std::endl;
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

This is a hobby project for me to have fun learning new things and tweaking the results.  If someone is interested, I'm more than happy to share the work/results with whomever wants to make use of it.

If you are interested in contributing however, please open an issue and we can discuss what it is you would like to tackle.  

Thanks!

## License
[MIT](https://choosealicense.com/licenses/mit/)

## Project Namesake(s)
-  Learn about: [Frederick II - Holy Roman Emperor](https://en.wikipedia.org/wiki/Frederick_II,_Holy_Roman_Emperor)
-  Learn about: [Frederick II - Prussia](https://en.wikipedia.org/wiki/Frederick_the_Great)