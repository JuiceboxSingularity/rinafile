#include <cstdlib>
#include <string>

#include <librina/librina.h>

#define RINA_PREFIX     "rina-echo-time"
#include <librina/logs.h>

//#include "config.h"
#include "server.h"

using namespace std;


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
