#include "pch.h"
#include <msclr\marshal_cppstd.h>
#include "Interface.h"
#include "Handler.h"
#include <string>
#include <iostream>
using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;

void send_cmd(string cmd) {
	std::cout << cmd << endl;
	Handler^ ins = Handler::get();
	String^ command = gcnew String(cmd.c_str());
	ins->execute(command);
	//ins->run_command(command);
}

bool idle() {
	Handler^ ins = Handler::get();
	return ins->idle();
	return true;
}

string get_result() {
	Handler^ ins = Handler::get();                                                                                                                                                                                                                                                                                                                                                         
	string s = msclr::interop::marshal_as<string>(ins->get_result());
	cout << s << endl;
	return s;
}

bool avail() {
	Handler^ ins = Handler::get();
	return ins->avail();
	//return true;
}