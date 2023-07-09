#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>


#include<winSock2.h>
#include<windows.h>
// #include<bits/stdc++.h>
#include <thread>

// #pragma comment(lib,"ws2_32.lib")//���Ӵ˶�̬���ӿ� windows����

using namespace std;

void recv_thread(int sock, ofstream& output_file) {
    while (true) {
        char buffer[64];
        int len = recv(sock, buffer, 64, 0);
        if (len <= 0) {
            continue;
        }
        float pos[2];
		sscanf(buffer, "%f %f", &pos[0], &pos[1]);
        cout << "    x: " << pos[0] << ";\t y: " << pos[1] << endl;
        output_file << pos[0] << " " << pos[1] << endl;
    }

}

int main()
{
	// initial windows socket 2,x
	WSADATA dat;
	if(0 != WSAStartup(MAKEWORD(2,2),&dat))
	{
		return 0;
	}

	// create a TCP socket
	SOCKET _mysocket = socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET == _mysocket)
    {
        return 0;
    }

    // set server
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(8886); // server's port number
	_sin.sin_addr.S_un.S_addr =  inet_addr("10.89.16.238"); // server's IP
	if(SOCKET_ERROR == connect(_mysocket,(sockaddr*)&_sin,sizeof(sockaddr_in)))
	{
		cout<<"Failed to connect to server"<<endl;
		closesocket(_mysocket);
	}
	else
	{
		cout<<"Server connected"<<endl;
	}

    unsigned long non_blocking = 1;
    int iResult = ioctlsocket(_mysocket, FIONBIO, &non_blocking);
    if (iResult == SOCKET_ERROR) {
        cout << "Failed to set socket to non-blocking mode: " << WSAGetLastError() << endl;
        closesocket(_mysocket);
    }


	ifstream input_file("2023-2-20-10-10-2_Decoder.txt", ios::in);
	ofstream output_file("new_7_9_robot_trajectory.txt", ios::out);

	// create thread for receive data
	thread t1(recv_thread,_mysocket, ref(output_file));
	t1.detach();

	for (int timeStep = 0; timeStep < 10; timeStep ++)
	{
	    // wait for 100ms
        this_thread::sleep_for(chrono::milliseconds(100));

	    string line;
        if (!getline(input_file, line)) {
            // read fail
            break;
        }
        // save the data to stringstream
        stringstream ss(line);

        // load all numbers
        float num;
        vector<float> nums;
        while (ss >> num) {
            nums.push_back(num);
        }

        // read the last two number
        float pos[2] = {nums[nums.size() - 2], nums[nums.size() - 1]};

        cout << "x: " << pos[0] << ";\t y: " << pos[1] << endl;

        // send positions
        char send_buf[64];
        sprintf(send_buf, "%0.6f %0.6f\n", pos[0], pos[1]);
        send(_mysocket, send_buf, strlen(send_buf), 0);
	}

    // wait for 1 second for receiving
    this_thread::sleep_for(chrono::milliseconds(1000));

	//close socket
	closesocket(_mysocket);

	//clear windows socket enviorment
	WSACleanup();

    // close files
    input_file.close();
    output_file.close();

	return 0;
}
