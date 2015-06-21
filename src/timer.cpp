#include "timer.h"

bool interrupt = false;
int check_interval = (size_t (1)<<10);
int check_time = 10*60;
int time_alert = 40*60; //second
int time_limit = 50*60; // second

TmUsage timer;
TmStat timing;

void reportTime()
{
	cout<<"================================== Timing =================================="<<endl;
	timer.getPeriodUsage(timing);
	cout<<"Period time elapsed : "<< (timing.uTime + timing.sTime) / 1000000.0 <<" s"<<endl;
	timer.getTotalUsage(timing);
	cout<<"Total time elapsed : "<< (timing.uTime + timing.sTime) / 1000000.0 <<" s"<<endl;
	cout<<"==================================        =================================="<<endl;
}
