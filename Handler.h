#pragma once
#include "pch.h"

using namespace System;
using namespace System::Threading;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Management::Automation;
using namespace System::Management::Automation::Runspaces;


public ref class Handler
{
public:
	static Handler^ instance;
private:
	bool is_idle;
	bool is_stop;
	String^ command;
	List<String^>^ results;
	Runspace^ runspace;
public:
	Handler();
	~Handler();
public:
	static Handler^ get();
public:
	bool initialize();
	void run_command();
    void execute(String^ cmd);
	String^ get_result();
	String^ get_command();
    void set_command(String^ cmd);
	bool idle();
	void stop();
	bool avail();
};

Handler^ Handler::get() {
    if (Handler::instance == nullptr)
    {
        Handler::instance = gcnew Handler();
        instance->initialize();
    }
    
    return Handler::instance;
}

Handler::Handler() :is_stop(false), is_idle(true) {

    Console::WriteLine("Handler Instantiated\n");
}

Handler::~Handler() {
    Console::WriteLine("Handler Destroyed\n");
}

bool Handler::initialize() {
    this->results = gcnew List<String^>();
    this->runspace = RunspaceFactory::CreateRunspace();
    this->runspace->Open();
    return true;
}

String^ Handler::get_command() {
    return this->command;
}

void Handler::set_command(String^ cmd) {
    this->command = cmd;
}

bool Handler::idle() {
    return this->is_idle;
}

bool Handler::avail() {
    return this->results->Count > 0;
}

String^ Handler::get_result() {
    String^ result = this->results[0];
    this->results->RemoveAt(0);
    return result;
}

void Handler::stop() {
    this->is_stop = true;
}



void Handler::run_command() {
    String^ command = this->command;
    bool emp = String::IsNullOrEmpty(command);
    if (emp || !this->is_idle) {
        return;
    }
    this->is_idle = false;
    String^ cmd = String::Concat(command, " | Out-String -Stream");
    Console::WriteLine(cmd);
    Pipeline^ pipe = this->runspace->CreatePipeline(cmd, true);
    pipe->InvokeAsync();
    PipelineReader<PSObject^>^ pipe_out = pipe->Output;
    PipelineReader<Object^>^ pipe_mst = pipe->Error;
    bool zip = true;
    bool sip = true;
   
    while (zip || sip)
    {
        if (!pipe_out->EndOfPipeline)
        {
            Collection<PSObject^>^ outs = pipe_out->NonBlockingRead();
            for (int i = 0; i < outs->Count; i++)
            {
                String^ result = outs[i]->ToString();
                this->results->Add(result);
                Console::WriteLine(result);
            }
        }
        if (!pipe_mst->EndOfPipeline)
        {
            Collection<Object^>^ outs = pipe_mst->NonBlockingRead();
            for (int i = 0; i < outs->Count; i++)
            {
                String^ result = outs[i]->ToString();
                this->results->Add(result);
                Console::WriteLine(result);
            }
        }

        PipelineState state = pipe->PipelineStateInfo->State;
        if (state == PipelineState::Failed)
        {
            String^ msg = pipe->PipelineStateInfo->Reason->Message;
            this->results->Add(msg);
            Console::WriteLine(msg);
        }
        if (is_stop) {
            pipe->Stop();
            this->is_stop = false;
        }
        zip = !pipe_out->EndOfPipeline;
        sip = !pipe_mst->EndOfPipeline;
    }
    is_idle = true;
    //return true;
}

void Handler::execute(String^ cmd) {
    this->command = cmd;
    Console::WriteLine("thread creating");
    ThreadStart^ ts = gcnew ThreadStart(this, &Handler::run_command);
    Thread^ thread = gcnew Thread(ts);
    Console::WriteLine("thread init");
    thread->Start();
    Console::WriteLine("thread submitted");
}

