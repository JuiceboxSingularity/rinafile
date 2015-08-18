#include <librina/librina.h>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace rina;

int main(){

	rina::initialize("INFO", "");

	//string app_name = "rina.apps.rinafile.client";
	string app_name = "rina.apps.rinafile.client";
	string app_instance = "1";

	//string server_name = "rina.apps.rinafile.server";
	string server_name = "rina.apps.echotime.server";
	string server_instance = "1";

	IPCEvent* event;
	FlowSpecification qosspec;
	unsigned int seqnum;

	seqnum = ipcManager->requestFlowAllocation(
		ApplicationProcessNamingInformation(app_name, app_instance),
		ApplicationProcessNamingInformation(server_name, server_instance),
		qosspec);

	for (;;) {
		event = ipcEventProducer->eventWait();
		if (event && event->eventType == ALLOCATE_FLOW_REQUEST_RESULT_EVENT
						&& event->sequenceNumber == seqnum) {
				cout << "Registered\n";
				cout << flush;
				break;
		}
	}

	AllocateFlowRequestResultEvent* afrrevent = dynamic_cast<AllocateFlowRequestResultEvent*>(event);

	rina::FlowInformation flow = ipcManager->commitPendingFlow(afrrevent->sequenceNumber,
															afrrevent->portId,
															afrrevent->difName);

	cout << flow.portId << "\n" << flush;

	char *buffer = "HELLO \0";

	while(true){
		ipcManager->writeSDU(flow.portId, buffer, 7);
		cout << "SENT PING\n";
		sleep(1);
	}
}
