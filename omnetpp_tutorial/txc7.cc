/*
 * txc7.cc
 *
 *  Created on: Jul 17, 2023
 *      Author: plexe
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * In the previous models, `tic' and `toc' immediately sent back the
 * received message. Here we'll add some timing: tic and toc will hold the
 * message for 1 simulated second before sending it back. In OMNeT++
 * such timing is achieved by the module sending a message to itself.
 * Such messages are called self-messages (but only because of the way they
 * are used, otherwise they are completely ordinary messages) or events.
 * Self-messages can be "sent" with the scheduleAt() function, and you can
 * specify when they should arrive back at the module.
 *
 * We leave out the counter, to keep the source code small.
 */

class Txc7 : public cSimpleModule
{
    private:
        cMessage *event; // pointer to the event object which we'll use for timing
        cMessage *tictocMsg; // variable to remember the message until we send it back

    public:
        Txc7();
        virtual ~Txc7();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Txc7);

Txc7::Txc7()
{
    // Set the pointer to nullptr, so that the destructor won't crash
    // even if initialize() doesn't get called because of a runtime
    // error or user cancellation during the startup process.
    event = tictocMsg = nullptr;
}

Txc7::~Txc7()
{
    // Dispose of dynamically allocated the objects
    cancelAndDelete(event);
    delete tictocMsg;
}

void Txc7::initialize()
{
    // arbitrary message for timing
    event = new cMessage("event");

    // No tictoc message yet.
    tictocMsg = nullptr;

    if (par("sendMsgOnInit").boolValue()) {
        // We don't start right away, but instead send an message to ourselves
        // (a "self-message") -- we'll do the first sending when it arrives
        // back to us, at t=5.0s simulated time.
        EV << "Scheduling first send to t=5.0s\n";
        tictocMsg = new cMessage("tictocMsg");
        scheduleAt(5.0, event);
    }
}

void Txc7::handleMessage(cMessage *msg)
{
    // There are several ways of distinguishing messages, for example by message
    // kind (an int attribute of cMessage) or by class using dynamic_cast
    // (provided you subclass from cMessage). In this code we just check if we
    // recognize the pointer, which (if feasible) is the easiest and fastest
    // method.
    if (msg == event) {
        // The self-message arrived, so we can send out tictocMsg and nullptr out
        // its pointer so that it doesn't confuse us later.
        EV << "Wait period is over, sending back message\n";
        send(tictocMsg, "out");
        tictocMsg = nullptr;
    }
    else {
        // "Lose" the message with 0.1 probability:
        if (uniform(0,1) < 0.1) {
            EV << "\"Losing\" message\n";
            bubble("message lost"); // making animation more informative...
            delete msg;
        }

        else {

            // Modeling a random processing delay

            // The "delayTime" module parameter can be set to values like
            // "exponential(5)" (tictoc7.ned, omnetpp.ini), and then here
            // we'll get a different delay every time.
            simtime_t delay = par("delayTime");

            // If the message we received is not our self-message, then it must
            // be the tic-toc message arriving from our partner. We remember its
            // pointer in the tictocMsg variable, then schedule our self-message
            // to come back to us in 1s simulated time.
            EV << "Message arrived, starting to wait " << delay << " sec...\n";
            tictocMsg = msg;
            scheduleAt(simTime()+delay, event);
        }


    }
}
