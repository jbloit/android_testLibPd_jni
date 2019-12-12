//
// Created by julien@macmini on 2019-12-09.
//

#include "Test.h"
#include <iostream>

#include "PdBase.hpp"
#include "PdObject.h"

#include <android/log.h>

#define  LOG_TAG    "testjni"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

extern "C" {
int libpd_init();
}


using namespace std;
using namespace pd;


Test::Test(){
//    prop = 12;
//    int a = libpd_init();
//    std::cout << " " << a << " \n";
//    std::cout << " " << a << " \n";


    // our pd engine
    PdBase pd;

    // one input channel, two output channels
    // block size 64, one tick per buffer
    float inbuf[64], outbuf[128];

    // custom receiver object for messages and midi
    PdObject pdObject;

    // init pd
    //
    // set 4th arg to true for queued message passing using an internal ringbuffer
    //
    // in this test, messages should return immediately when not queued otherwise
    // they should all return at once when pd is processing at the end of this
    // function
    //
    int srate = 44100;
    if(!pd.init(1, 2, srate, true)) {
        cerr << "Could not init pd" << endl;
        exit(1);
    }

    int midiChan = 1; // midi channels are 0-15

    // subscribe to receive source names
    pd.subscribe("toCPP");
    pd.subscribe("env");

    // set receivers
    pd.setReceiver(&pdObject);
    pd.setMidiReceiver(&pdObject);

    // add the data/pd folder to the search path
    pd.addToSearchPath("pd/lib");

    // audio processing on
    pd.computeAudio(true);


    cout << endl << "BEGIN Patch Test" << endl;

    // open patch
    Patch patch = pd.openPatch("pd/test.pd", ".");
    cout << patch << endl;

    // close patch
    pd.closePatch(patch);
    cout << patch << endl;

    // open patch again
    patch = pd.openPatch(patch);
    cout << patch << endl;

    // process any received messages
    //
    // in a normal case (not a test like this), you would call this in
    // your application main loop
    pd.processFloat(1, inbuf, outbuf);
    pd.receiveMessages();

    cout << "FINISH Patch Test" << endl;


    cout << endl << "BEGIN Message Test" << endl;

    ALOG("BEGIN Message Test");

    // test basic atoms
    pd.sendBang("fromCPP");
    pd.sendFloat("fromCPP", 100);
    pd.sendSymbol("fromCPP", "test string");

    // stream interface
    pd << Bang("fromCPP")
       << Float("fromCPP", 100)
       << Symbol("fromCPP", "test string");

    // send a list
    pd.startMessage();
    pd.addFloat(1.23);
    pd.addSymbol("a symbol");
    pd.finishList("fromCPP");

    // send a message to the $0 receiver ie $0-toOF
    pd.startMessage();
    pd.addFloat(1.23);
    pd.addSymbol("a symbol");
    pd.finishList(patch.dollarZeroStr()+"-fromCPP");

    // send a list using the List object
    List testList;
    testList.addFloat(1.23);
    testList.addSymbol("sent from a List object");
    pd.sendList("fromCPP", testList);
    pd.sendMessage("fromCPP", "msg", testList);

    // stream interface for list
    pd << StartMessage() << 1.23 << "sent from a streamed list" << FinishList("fromCPP");

    cout << "FINISH Message Test" << endl;


    cout << endl << "BEGIN MIDI Test" << endl;

    // send functions
    pd.sendNoteOn(midiChan, 60);
    pd.sendControlChange(midiChan, 0, 64);
    pd.sendProgramChange(midiChan, 100);   // note: [pgmin] range is 1 - 128
    pd.sendPitchBend(midiChan, 2000);   // note: libpd uses -8192 - 8192 while [bendin] returns 0 - 16383,
    // so sending a val of 2000 gives 10192 in pd
    pd.sendAftertouch(midiChan, 100);
    pd.sendPolyAftertouch(midiChan, 64, 100);
    pd.sendMidiByte(0, 239);    // note: pd adds +2 to the port number from [midiin], [sysexin], & [realtimein]
    pd.sendSysex(0, 239);       // so sending to port 0 gives port 2 in pd
    pd.sendSysRealTime(0, 239);

    // stream
    pd << NoteOn(midiChan, 60) << ControlChange(midiChan, 100, 64)
       << ProgramChange(midiChan, 100) << PitchBend(midiChan, 2000)
       << Aftertouch(midiChan, 100) << PolyAftertouch(midiChan, 64, 100)
       << StartMidi(0) << 239 << Finish()
       << StartSysex(0) << 239 << Finish()
       << StartSysRealTime(0) << 239 << Finish();

    cout << "FINISH MIDI Test" << endl;


    cout << endl << "BEGIN Array Test" << endl;

    // array check length
    cout << "array1 len: " << pd.arraySize("array1") << endl;

    // read array
    std::vector<float> array1;
    pd.readArray("array1", array1);	// sets array to correct size
    cout << "array1 ";
    for(int i = 0; i < array1.size(); ++i)
        cout << array1[i] << " ";
    cout << endl;

    // write array
    for(int i = 0; i < array1.size(); ++i)
        array1[i] = i;
    pd.writeArray("array1", array1);

    // ready array
    pd.readArray("array1", array1);
    cout << "array1 ";
    for(int i = 0; i < array1.size(); ++i)
        cout << array1[i] << " ";
    cout << endl;

    // clear array
    pd.clearArray("array1", 10);

    // ready array
    pd.readArray("array1", array1);
    cout << "array1 ";
    for(int i = 0; i < array1.size(); ++i)
        cout << array1[i] << " ";
    cout << endl;

    cout << "FINISH Array Test" << endl << endl;


    cout << "BEGIN PD Test" << endl;
    pd.sendSymbol("fromCPP", "test");
    cout << "FINISH PD Test" << endl << endl;


    // play a tone by sending a list
    // [list tone pitch 72 (
    pd.startMessage();
    pd.addSymbol("pitch");
    pd.addFloat(72);
    pd.finishList("tone");
    pd.sendBang("tone");

    // now run pd for ten seconds (logical time)
    // you should see all the messages from pd print now
    // since processFloat actually runs the pd dsp engine and the recieve
    // functions pass messages to our PdObject
    cout << "Processing PD" << endl;
    for(int i = 0; i < 10 * srate / 64; i++) {
        // fill inbuf here
        pd.processFloat(1, inbuf, outbuf);
        pd.receiveMessages();
        pd.receiveMidi();
        // use outbuf here
    }

    // be nice and clean up on exit
    pd.closePatch(patch);
    pd.computeAudio(false);


}

Test::~Test(){
}

int Test::getProp(){
    return  prop;
}