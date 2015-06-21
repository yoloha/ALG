#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <stdio.h>
#include "tm_usage.h"
using namespace std;
using namespace CommonNs;

extern bool interrupt;
extern int check_interval;
extern int check_time;
extern int time_alert;
extern int time_limit;
extern TmUsage timer;
extern TmStat timing;

inline bool checkTimeElapsed(bool first)
{
	static long start = 0;
	timer.getTotalUsage(timing);
	if(first==true){
		start = (timing.uTime + timing.sTime) / 1000000.0;
	}
	else{
		long end = (timing.uTime + timing.sTime) / 1000000.0;
		if (end - start > check_time){
			

			for (int i = 0; i < 80; ++i)
			{
				cout<<"\r";
				
			}
			fflush(stdout);
			cout<<"================================== Check =================================="<<endl;
			cout<<"Longer than "<<check_time<<" s"<<endl;
			cout<<"==================================       =================================="<<endl;
			return false;
		}
	}
	return true;
}

inline void checkTimeLeft()
{
	timer.getTotalUsage(timing);
	long total = (timing.uTime + timing.sTime) / 1000000.0;
	if ( total>time_alert ){
		time_alert += 1*60;
		cout<<"================================== Alert =================================="<<endl;
		cout<<"Time elapsed : "<<total<<" s"<<endl;
		cout<<"Time left : "<<time_limit-total<<" s"<<endl;
		cout<<"==================================       =================================="<<endl;
	}
	if ( total>time_limit ){
		cout<<"================================== Interrupt =================================="<<endl;
		cout<<"Interrupt program at "<<total<<" s"<<endl;
		interrupt=true;
		cout<<"==================================           =================================="<<endl;
	}
}

void reportTime();

#endif
