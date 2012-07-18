/**
* ResThread.cpp - Helper worker thread to handle image processing and storing
* @author Nikko Schaff
*/

#include "ResThread.h"

using namespace std;
using namespace gsweb;

typedef void* (*thread_f)(void*);

ResThread::ResThread( std::string& fileName, int numQuestions, bool readName )
    : fileName( fileName ),
        numQuestions( numQuestions ),
        readName( readName ),
        threadDone( false )
{
    if ( pthread_create( &myThread, NULL,
                                   (thread_f) &ResThread::implThread, this ) ) {
        threadDone = true;
    }
}

ResThread::~ResThread()
{}

void ResThread::join()
{
    if ( !threadDone ) {
        pthread_join( myThread, NULL );
        threadDone = true;
    }
}

const ResThread::ResultValue* ResThread::getResult() const
{
    return &result;
}


void* ResThread::implThread( ResThread* r )
{
    r->result = r->imgReader.readImage(
                                    r->fileName, r->numQuestions, r->readName );
    return NULL;
}

ResGroup::~ResGroup()
{
    removeThreads();
}

void ResGroup::addThread( ResThread* thread )
{
    myThreads.push_back( thread );
}

void ResGroup::addThread( std::string& fileName, int numQuestions, bool readName )
{
    addThread( new ResThread( fileName, numQuestions, readName ) );
}

bool ResGroup::removeThread( ResThread* thread )
{
    list<ResThread*>::iterator it;
    for ( it = myThreads.begin(); it != myThreads.end(); ++it ) {
        if ( *it == thread ) {
            myThreads.erase( it );
            return true;
        }
    }
    return false;
}

void ResGroup::removeThreads()
{
    list<ResThread*>::iterator it;
    for ( it = myThreads.begin(); it != myThreads.end(); ++it ) {
        delete *it;
    }
    myThreads.clear();
}

void ResGroup::getResults( vector<const ResThread::ResultValue*>& ret ) {
    ret.clear();
    list<ResThread*>::iterator it;
    for ( it = myThreads.begin(); it != myThreads.end(); ++it ) {
        ret.push_back( (*it)->getResult() );
    }
}

int ResGroup::size() const
{
    return int(myThreads.size());
}

void ResGroup::join()
{
    list<ResThread*>::iterator it;
    for ( it = myThreads.begin(); it != myThreads.end(); ++it ) {
        (*it)->join();
    }
}