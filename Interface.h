#pragma once
#include <string>
#include "pch.h"
#include <string>
using namespace std;

void send_cmd(string cmd);

bool idle();

std::string get_result();

bool avail(); 