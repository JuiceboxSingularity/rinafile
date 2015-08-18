//
// Echo Server
// 
// Addy Bombeke <addy.bombeke@ugent.be>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
#include <string>
#include <cstdlib>

#include <iostream>

#define RINA_PREFIX     "rina-echo-app"
#include <librina/logs.h>

#include "server.h"

using namespace std;
using namespace rina;

Server::Server(const string& t_type,
               const string& dif_name,
               const string& app_name,
               const string& app_instance,
               const int perf_interval,
               const int dealloc_wait) :
		appName(app_name), appInstance(app_instance), difName(dif_name),	
        test_type(t_type), interval(perf_interval), dw(dealloc_wait)
{
}

void Server::run()
{
		
		ApplicationRegistrationInformation ari;
        RegisterApplicationResponseEvent *resp;
        unsigned int seqnum;
        IPCEvent *event;

        ari.ipcProcessId = 0;  // This is an application, not an IPC process
        ari.appName = ApplicationProcessNamingInformation(appName,
                                                          appInstance);
        if (difName == string()) {
                ari.applicationRegistrationType =
                        rina::APPLICATION_REGISTRATION_ANY_DIF;
        } else {
                ari.applicationRegistrationType =
                        rina::APPLICATION_REGISTRATION_SINGLE_DIF;
                ari.difName = ApplicationProcessNamingInformation(difName, string());
        }

        // Request the registration
        seqnum = ipcManager->requestApplicationRegistration(ari);

        // Wait for the response to come
        for (;;) {
                event = ipcEventProducer->eventWait();
                if (event && event->eventType ==
                                REGISTER_APPLICATION_RESPONSE_EVENT &&
                                event->sequenceNumber == seqnum) {
                        break;
                }
        }

        resp = dynamic_cast<RegisterApplicationResponseEvent*>(event);

        // Update librina state
        if (resp->result == 0) {
                ipcManager->commitPendingRegistration(seqnum, resp->DIFName);
        } else {
                ipcManager->withdrawPendingRegistration(seqnum);
                throw ApplicationRegistrationException("Failed to register application");
        }	

        
		rina::FlowInformation flow;

        for(;;) {
                IPCEvent* event = ipcEventProducer->eventWait();
                int port_id = 0;
                DeallocateFlowResponseEvent *resp = NULL;

                if (!event)
                        return;

                switch (event->eventType) {

                case REGISTER_APPLICATION_RESPONSE_EVENT:
                        ipcManager->commitPendingRegistration(event->sequenceNumber,
                                                             dynamic_cast<RegisterApplicationResponseEvent*>(event)->DIFName);
                        break;

                case UNREGISTER_APPLICATION_RESPONSE_EVENT:
                        ipcManager->appUnregistrationResult(event->sequenceNumber,
                                                            dynamic_cast<UnregisterApplicationResponseEvent*>(event)->result == 0);
                        break;

                case FLOW_ALLOCATION_REQUESTED_EVENT:
                        flow = ipcManager->allocateFlowResponse(*dynamic_cast<FlowRequestEvent*>(event), 0, true);
                        port_id = flow.portId;
                        LOG_INFO("New flow allocated [port-id = %d]", port_id);
                        
						startWorker(port_id);

						break;

                case FLOW_DEALLOCATED_EVENT:
                        port_id = dynamic_cast<FlowDeallocatedEvent*>(event)->portId;
                        ipcManager->flowDeallocated(port_id);
                        LOG_INFO("Flow torn down remotely [port-id = %d]", port_id);
                        break;

                case DEALLOCATE_FLOW_RESPONSE_EVENT:
                        LOG_INFO("Destroying the flow after time-out");
                        resp = dynamic_cast<DeallocateFlowResponseEvent*>(event);
                        port_id = resp->portId;

                        ipcManager->flowDeallocationResult(port_id, resp->result == 0);
                        break;

                default:
                        LOG_INFO("Server got new event of type %d",
                                        event->eventType);
                        break;
                }
        }
}

void Server::startWorker(int port_id){

	uint maxbuffer = 255;
	char *buffer = new char[maxbuffer];

	try {
		while (true){
			int bytes_read = ipcManager->readSDU(port_id,buffer,maxbuffer);
			ipcManager->writeSDU(port_id,buffer,bytes_read);
			cout << buffer << flush;
		}
	} catch (rina::IPCException e){
			cout << "ERROR ERROR ERROR\n";
	}
        
}

void Server::destroyFlow(sigval_t val)
{
        int port_id = val.sival_int;

        // TODO here we should store the seqnum (handle) returned by
        // requestFlowDeallocation() and match it in the event loop
        try {
        	ipcManager->requestFlowDeallocation(port_id);
        } catch(rina::Exception &e) {
        	//Ignore, flow was already deallocated
        }
}

int wrapped_main(int argc, char** argv)
{
        bool listen;
        bool registration;
        bool quiet;
        unsigned int count;
        unsigned int size;
        unsigned int wait;
        int gap;
        int perf_interval = -1;
        int dw = -1;
        string test_type = "ping";
        string server_apn = "rina.apps.echotime.server";
        string server_api = "1";
        string dif_name = "";
        
        rina::initialize("INFO", "");

               
        Server s(test_type, dif_name, server_apn, server_api,
                                perf_interval, dw);
        s.run();

}

int main(int argc, char * argv[])
{
        int retval;

        try {
                retval = wrapped_main(argc, argv);
        } catch (rina::Exception& e) {
                LOG_ERR("%s", e.what());
                return EXIT_FAILURE;

        } catch (std::exception& e) {
                LOG_ERR("Uncaught exception");
                return EXIT_FAILURE;
        }

        return retval;
}

