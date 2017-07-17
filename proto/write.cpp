#include "msg.pb.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace txh;

int main(void)
{
	helloProto msg;
	msg.set_id(100);
	msg.set_str("txh");
	fstream output("./msg.pb",ios::out | ios::trunc | ios::binary);
	if(!msg.SerializeToOstream(&output))
	{
		cerr << "failed to write msg." << endl;
		return -1;	
	}
	return 0;

}
