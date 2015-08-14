
#include <librina/librina.h>
#include <iostream>

using namespace std;
using namespace rina;

int main(){
	rina::ApplicationRegistrationInformation ari;
	
	ari.ipcProcessId = 0;
	
	std::string app_name = "rina.apps.rinafile.server";
	std::string app_instance = "1";
	ari.appName = rina::ApplicationProcessNamingInformation(app_name, app_instance);

	ari.applicationRegistrationType = rina::APPLICATION_REGISTRATION_ANY_DIF;
	
	//ari.applicationRegistrationType = ApplicationRegistrationType::APPLICATION_REGISTRATION_SINGLE_DIF;
	//ari.difName = ApplicationProcessNamingInformation(dif_name, string());
	

	
	unsigned int seqnum = ipcManager->requestApplicationRegistration(ari);
	IPCEvent *event;
	RegisterApplicationResponseEvent *resp;
	
	for (;;) {
		event = ipcEventProducer->eventWait();
		if (event && event->eventType == REGISTER_APPLICATION_RESPONSE_EVENT
		&& event->sequenceNumber == seqnum) break;
	}
	
	resp = dynamic_cast<RegisterApplicationResponseEvent*>(event);

	if (resp->result == 0) {
		ipcManager->commitPendingRegistration(seqnum, resp->DIFName);
	} else {
		ipcManager->withdrawPendingRegistration(seqnum);
		throw ApplicationRegistrationException("Failed to register application");
	}

	cout << "REGISTERED\n";

	rina::FlowInformation flow;
	int buffer_size = 255;
	char *buffer = new char[buffer_size];

	while(true){
	
		event = ipcEventProducer->eventWait();
		int port_id = 0;
		cout << "REGISTERED\n";
		DeallocateFlowResponseEvent *resp = NULL;

		if (!event)
			std::cout << "NULL EVENT";
			return -1;

		switch (event->eventType){
		case FLOW_ALLOCATION_REQUESTED_EVENT:
				flow = ipcManager->allocateFlowResponse(*dynamic_cast<FlowRequestEvent*>(event), 0, true);
				port_id = flow.portId;
				
				try {
					for(;;){
						int bytes_read = ipcManager->readSDU(port_id,buffer,buffer_size);
						cout << bytes_read;
						cout << buffer;
					}
				} catch (rina::IPCException e){

				}

		}
	}
}
