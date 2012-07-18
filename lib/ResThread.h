/**
* ResThread.h - Helper worker thread to handle image processing and storing
* @author Nikko Schaff
*/

#ifndef ResThread_H_
#define ResThread_H_

#include <pthread.h>
#include <vector>
#include <list>

#include "ImageReader.h"

namespace gsweb {

    class ResThread {

    public:

        typedef std::vector<std::vector<float> > ResultValue;

        ResThread( std::string& fileName, int numQuestions, bool readname );

        virtual ~ResThread();

        void join();

        const ResultValue* getResult() const;

    private:

        ImageReader imgReader;

        ResultValue result;

        std::string fileName;

        int numQuestions;

        bool readName;

        bool threadDone;

        pthread_t myThread;

        static void* implThread( ResThread* r );

    };

    class ResGroup {

    public:

        virtual ~ResGroup();

        void addThread( ResThread* thread );

        void addThread( std::string& filename, int numQuestions, bool readname );

        bool removeThread( ResThread* thread );

        void removeThreads();

        void getResults( std::vector<const ResThread::ResultValue*>& ret );

        int size() const;

        void join();

    private:

        std::list<ResThread*> myThreads;

    };

}

#endif