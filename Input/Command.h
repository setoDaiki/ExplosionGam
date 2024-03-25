#pragma once
#include <memory>

class Command {
public:
	virtual ~Command(){};
	virtual void Excute() = 0;

};


using CommandPtrU = std::unique_ptr<Command>;
