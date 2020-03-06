// EasiBuild Mk 2 Firmware: controllers framework
// Author: Matt Gumbley M0CUV <matt.gumbley@devzendo.org>

#ifndef _CONTROLLERS_H_
#define _CONTROLLERS_H_

class Controller {
public:
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void processEvent(int event) = 0;
    virtual char *name() = 0;
};

class ControllerStack {
public:
    ControllerStack() {
        reset();
    }
    
    void reset() {
        int i;
        for (i=0; i<maxControllers; i++) {
            controllerStack[i] = NULL;
        }
        topOfStack = -1;
    }

    void push(Controller *c) {
        if (topOfStack < maxControllers) {
            if (topOfStack >= 0) {
                controllerStack[topOfStack]->exit();
            }
            AVR_ATOMIC {
                controllerStack[++topOfStack] = c;
            }
            sprintf(evdispatchbuf, "push ctrl:0x%04X (%s)", c, c->name());
            Serial.println(evdispatchbuf);
            c->enter();
        } else {
            Serial.println("ControllerStack overflow");
        }
    }
    
    Controller *pop() {
        if (topOfStack == -1) {
            Serial.println("ControllerStack underflow");
            return NULL;
        }
        Controller *oldHead = controllerStack[topOfStack];
        oldHead->exit();
        AVR_ATOMIC {
            controllerStack[topOfStack--] = NULL;
        }
        if (topOfStack != -1) {
            controllerStack[topOfStack]->enter();
        }
        return oldHead;
    }
    
    void processEvent(int event) {
//        Serial.print("ControllerStack got event 0x");
//        Serial.print(event, HEX);
//        Serial.print(" topOfStack is ");
//        Serial.print(topOfStack);
//        Serial.println("");
        if (topOfStack != -1) {
            sprintf(evdispatchbuf, "disp ev:0x%04X to:0x%04X (%s)", event, controllerStack[topOfStack], controllerStack[topOfStack]->name());
            Serial.println(evdispatchbuf);
            controllerStack[topOfStack]->processEvent(event);
        } else {
            Serial.println("No controller to service event");
        }
    }

private:
    char evdispatchbuf[80];
    static const int maxControllers = 10;
    Controller *controllerStack[maxControllers];
    int topOfStack;
};



ControllerStack controllerStack;

defineTaskLoop(Task1) {
    int event;
    if (eventFifo.get(&event)) {
        //Serial.println("event to stack");
        controllerStack.processEvent(event);
    }
    yield();
}

#endif

