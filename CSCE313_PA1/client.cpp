/*
	Author of the starter code
    Yifan Ren
    Department of Computer Science & Engineering
    Texas A&M University
    Date: 9/15/2024
	
	Please include your Name, UIN, and the date below
	Name: Adam Hussain
	UIN: 632002654
	Date: 09/20/2024
*/
#include "common.h"
#include "FIFORequestChannel.h"
#include <iostream>
#include <fstream>
#include <iomanip>


using namespace std;


int main (int argc, char *argv[]) {
	int opt;
	int p = -1;
	double t = -1.0;
	int e = -1;
	vector<FIFORequestChannel*> channels = vector<FIFORequestChannel*>();


	//changel defs from p = 0, t = 0.0, e = 1 to p = -1, t = -1.0, e = -1

	int msgSize = MAX_MESSAGE;
	int new_channel = false;

	string filename = "";

	//Add other arguments here
	while ((opt = getopt(argc, argv, "p:t:e:f:m:c")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi (optarg);
				break;
			case 't':
				t = atof (optarg);
				break;
			case 'e':
				e = atoi (optarg);
				break;
			case 'f':
				filename = optarg;
				break;
			case 'm':
				msgSize = atoi(optarg);
				break;
			case 'c':
				new_channel = true;
				break;
		}
	}

	//Task 1:
	//Run the server process as a child of the client process
	pid_t pid = fork();

	char* args[] = {(char*) "./server", (char*) "-m", (char*) to_string(msgSize).c_str(), nullptr};

	if (pid == 0){ // child
		execvp("./server", args);
	}

	FIFORequestChannel cont_chan("control", FIFORequestChannel::CLIENT_SIDE);
	channels.push_back(&cont_chan);

	//Task 4:
	//Request a new channel
	if (new_channel) {
		// send newchannel request to the server
		MESSAGE_TYPE nc = NEWCHANNEL_MSG;
		cont_chan.cwrite(&nc, sizeof(MESSAGE_TYPE));
		// create a variabble to hold the name
		char* name = new char[MAX_MESSAGE];
		// cread the response from the server
		cont_chan.cread(name, MAX_MESSAGE);
		// call the FIFORequestChannel constructor with the name from the sERVER
		FIFORequestChannel* newChan = new FIFORequestChannel(name, FIFORequestChannel::CLIENT_SIDE);
		// Push the new channel to the vector
		channels.push_back(newChan);
		delete[] name;
	}
	
	
	//Task 2:
	//Request data points
	FIFORequestChannel chan = *(channels.back());
	// single data point
	if(p!= -1 && t!= -1 && e!= -1){ // if p t e have been specified
		char buf[MAX_MESSAGE];
		datamsg x(p,t,e);
		memcpy(buf, &x, sizeof(datamsg));
		chan.cwrite(buf, sizeof(datamsg));
		double reply;
		chan.cread(&reply, sizeof(double));
		cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;
	}
	// if only person req 1k datapoints
	// loop over 1k
	// send req for ecg 1
	// send req for ecg 2
	// write line to recieved/x1.csv
	else if(p!= -1){ 
		ofstream file;
		file.open("received/x1.csv");
		t = 0;
		for (int i = 0; i < 1000;i++){

			char buf[MAX_MESSAGE];
			datamsg x1(p,t,1);
			file << t << ',';

			memcpy(buf, &x1, sizeof(datamsg));
			chan.cwrite(buf, sizeof(datamsg));
			double reply1;
			chan.cread(&reply1, sizeof(double));
			file << reply1 << ',';

			datamsg x2(p,t,2);
			memcpy(buf, &x2, sizeof(datamsg));
			chan.cwrite(buf, sizeof(datamsg));
			double reply2;
			chan.cread(&reply2, sizeof(double));
			file << reply2 << endl;

    		t += 0.004;
		}
		file.close();
	}
    
	
	//Task 3:
	//Request files
	if(!filename.empty()){
		filemsg fm(0, 0);

		int len = sizeof(filemsg) + (filename.size() + 1);
		char* buf2 = new char[len];
		memcpy(buf2, &fm, sizeof(filemsg));
		strcpy(buf2 + sizeof(filemsg), filename.c_str());
		chan.cwrite(buf2, len);

	
		__int64_t file_length;
		chan.cread(&file_length, sizeof(__int64_t));
		cout << "The length of " << filename << " is " << file_length << endl;

		ofstream file;
		file.open("received/"+filename, std::ios_base::binary);

		char* buf3 = new char[msgSize]; // biggest possible msg

		// loop over segments in file(filesize/buff capacity(m))
		int segments = file_length/msgSize;
		for(int i = 0;i<=segments;i++){


			filemsg* file_req = (filemsg*)buf2;
			file_req->offset = i*msgSize;
			file_req->length = msgSize;


			//  if the last segment is smaller than the max buffer size, must adjust length to only requerst the remaining bytes.
			if (msgSize >= file_length - file_req->offset)
				file_req->length = file_length - file_req->offset;
		
			//send the request (buff2)
			chan.cwrite(buf2, len);
			//recieve the response
			chan.cread(buf3, file_req->length);

			//write buf3 into file
			file.write(buf3, file_req->length);
		}



		delete[] buf2;
		delete[] buf3;
		file.close();
	}
	// filemsg fm(0, 0);
	// string fname = "1.csv";
	
	// int len = sizeof(filemsg) + (fname.size() + 1);
	// char* buf2 = new char[len];
	// memcpy(buf2, &fm, sizeof(filemsg));
	// strcpy(buf2 + sizeof(filemsg), fname.c_str());
	// chan.cwrite(buf2, len);

	// delete[] buf2;
	// __int64_t file_length;
	// chan.cread(&file_length, sizeof(__int64_t));
	// cout << "The length of " << fname << " is " << file_length << endl;
	
	//Task 5:
	// Closing all the channels
	MESSAGE_TYPE m = QUIT_MSG;
	if(new_channel){
		// do close and deletes
		cont_chan.cwrite(&m, sizeof(MESSAGE_TYPE));
	}
    chan.cwrite(&m, sizeof(MESSAGE_TYPE));
}
