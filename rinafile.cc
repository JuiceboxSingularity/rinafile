
#include <librina/librina.h>
#include <iostream>

using namespace std;
using namespace rina;

int main(){
	rina::ApplicationRegistrationInformation ari;
	
	ari.ipcProcessId = 0;
	std::string x;
	if (x == std::string()){
		std::cout << "????";
	}
	
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
	
	/*
	resp = dynamic_cast<RegisterApplicationResponseEvent*>(event);

	if (resp->result == 0) {
		ipcManager->commitPendingRegistration(seqnum, resp->DIFName);
	} else {
		ipcManager->withdrawPendingRegistration(seqnum);
		throw ApplicationRegistrationException("Failed to register application");
	}
	*/	
	while(true);
}
