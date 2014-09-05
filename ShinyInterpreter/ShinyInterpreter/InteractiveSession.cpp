#include "stdafx.h"

#include "InteractiveSession.h"

#include "Reader.h"
#include "Evaluator.h"
#include "Object.h"

#include <iostream>
#include <string>

InteractiveSession::InteractiveSession()
{
}

InteractiveSession::~InteractiveSession()
{
}

void InteractiveSession::Run()
{
    OnStart();
    MainLoop();
    OnStop();
}

void InteractiveSession::MainLoop()
{
    // Interpreter initialize.
    Evaluator evaluator;

    while (true)
    {   
        Reader reader;
        Object * pResult = evaluator.Evaluate(reader.Read(GetUserInput()));

        if (pResult != nullptr)
        {
            std::cout << pResult->ToString() << std::endl;
        }

        delete pResult;
    }

}

std::string InteractiveSession::GetUserInput() const
{
    std::string input;

    std::cout << "> ";
    std::cin >> input;

    return input;
}

void InteractiveSession::OnStart()
{
    std::cout << "Welcome to Shiny, a simple scheme inspired language. Use ctrl+c to exit." << std::endl;
}

void InteractiveSession::OnStop()
{
    std::cout << "Goodbye!" << std::endl;
}