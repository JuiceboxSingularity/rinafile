#ifndef SERVER_HPP
#define SERVER_HPP

#include <librina/librina.h>
#include <time.h>
#include <signal.h>

class Server
{
public:
        Server(const std::string& test_type,
               const std::string& dif_name,
               const std::string& app_name,
               const std::string& app_instance,
               const int perf_interval,
               const int dealloc_wait);

        void run();

protected:
        void servePingFlow(int port_id);
        static void destroyFlow(sigval_t val);

private:
        std::string test_type;
        int interval;
        int dw;
		std::string appName;
		std::string appInstance;
		std::string difName;

        void startWorker(int port_id);
        void printPerfStats(unsigned long pkt, unsigned long bytes,
                unsigned long us);
};

#endif
