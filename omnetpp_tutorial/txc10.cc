/*
 * txc10.cc
 *
 *  Created on: Aug 6, 2023
 *      Author: plexe
 */


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * module 0 generates a message, and the others
 * keep tossing it around in random directions until it arrives at
 * module 3.
 */
class Txc10 : public cSimpleModule
{
    protected:
        virtual void forwardMessage(cMessage *msg);
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Txc10);

void Txc10::initialize()
{
    if (getIndex() == 0) {
        // Boot the process scheduling the initial message as a self-message.
        char msgname[20];
        sprintf(msgname, "tic-%d", getIndex());
        cMessage *msg = new cMessage(msgname);
        scheduleAt(0.0, msg);
    }
}

void Txc10::handleMessage(cMessage *msg)
{
    if (getIndex() == 3) {
        // Message arrived.
        EV << "Message " << msg << " arrived.\n";
        delete msg;
    }
    else {
        // Need to forward the message.
        forwardMessage(msg);
    }
}

void Txc10::forwardMessage(cMessage *msg)
{
    // Pick a random gate to send it on.
    int n = gateSize("out");
    int k = intuniform(0, n-1);

    EV << "Forwarding message " << msg << " to port out [" << k << "]\n";
    send(msg, "out", k);
}
