/*
 * txc5.cc
 *
 *  Created on: Jul 17, 2023
 *      Author: plexe
 */


#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;


class Txc5 : public cSimpleModule
{
    private:
        int counter;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

// The module class needs to be registered with OMNeT++
Define_Module(Txc5);

void Txc5::initialize()
{
    // Initialize the counter with the "limit" module parameter, declared
    // in the NED file (tictoc5.ned).
    counter = par("limit");


    WATCH(counter);

    if (par("sendMsgOnInit").boolValue()) {
        EV << "Sending initial message\n";
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
    }
}

void Txc5::handleMessage(cMessage *msg)
{
    counter--;
    if (counter == 0) {
        EV << getName() << "'s counter reached zero, deleting message\n";
        delete msg;
    }
    else {
        EV << getName() << "'s counter is " << counter << ", sending it out again";
        send(msg, "out"); // send out the message
    }
}


