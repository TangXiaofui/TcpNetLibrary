#include "msg.pb.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace txh;


void listmsg(const helloProto &msg)
{
	cout << msg.id() << endl;
	cout << msg.str() << endl;	
}

int main(void)
{
	helloProto msg;
	fstream input("./msg.pb",ios::in | ios::binary);
	if(!msg.ParseFromIstream(&input)){
		cerr << "Failed to parse address book." << endl;
		return -1;	
	}
	listmsg(msg);
	return 0;	
}
