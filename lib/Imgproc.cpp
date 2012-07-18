// Include the Ruby headers and goodies
#include <cassert>
#include "ruby.h"
#include <vector>
#include "ImageReader.h"
#include <string>
#include "Imgproc.h"

using namespace std;
//using namespace boost::lambda;
//using namespace gsweb;

typedef VALUE (*rubyf)(...);

// Defining a space for information and references about the module
// to be stored internally
VALUE irm = Qnil;

// Convenience method
// The initialization method for this module
extern "C" void Init_Imgproc() {
	irm = rb_define_class("Imgproc", rb_cObject);
	rb_define_method(irm, "initialize", (rubyf)  method_init, 0);
	rb_define_method(irm, "readFiles", (rubyf) method_readFiles, 3);	
	rb_define_method(irm, "prepShowImage", (rubyf) method_prepShowImage, 2);
}

// Main initialization method used by ruby (".new")
extern "C" VALUE method_init(VALUE self) {
	return self;
}

/**
 * method_readFiles - main in method for Imgproc.  Reads and returns results
 *
 * @param	self	ruby-required module (not included in ruby method call)
 * @param 	rubyfilenames	The ruby-formatted string array of filenames
 *	(including extension and path!!!)
 * @param	rubynumQ	ruby-formatted number of questions on test
 * @param	rubyReadname	ruby bool value to determine if name to be read
 *
*/
extern "C" VALUE method_readFiles(VALUE self, VALUE rubyfilenames,
 VALUE rubynumQ, VALUE rubyReadname) {
	int numQ = NUM2INT( rubynumQ );
	int numFiles = int( RARRAY_LEN( rubyfilenames ) );
	bool readName = RTEST( rubyReadname );

	// c-style array of filenames
	std::vector<std::string> filenames;
	for( long i = 0; i < numFiles; i++ ) {
		VALUE rubyfn = rb_ary_entry(rubyfilenames,i);
		std::string strfname( StringValueCStr( rubyfn ) );
		filenames.push_back( strfname );
	}

	ImageReader imr;
	std::vector< std::vector< std::vector< float > > > results( numFiles );
    // asynchronize image reads
    //ResGroup group;
	//vector<const ResThread::ResultValue*> results;
	vector< vector< float > > res;
	for(  int i = 0; i < numFiles; i++ ) {
        //group.addThread( filenames[i], numQ, readName );
		res =  imr.readImage( filenames[i], numQ, readName );
		results[i] = res;
	}
    //group.join();
    //group.getResults( results );

    assert( numFiles == int(results.size()) );

	VALUE rbStudents = rb_ary_new();
	// Go through each for each student
	for( int i = 0; i < numFiles; i++ ) {
		// Go through each for each student's answers
		VALUE rbStudentAnswers = rb_ary_new();
		int sz = int( results[i].size() );
		for( int k = 0; k < sz; k++ ) {
			int wsize = int( (results[i])[k].size() );
			VALUE rbStudentAnswerComponents = rb_ary_new();
			for( int w = 0; w < wsize; w++ ) {
				VALUE ansDouble = DBL2NUM( 
                    (results[i])[k][w] );
				rb_ary_push( rbStudentAnswerComponents, ansDouble );
			}
			rb_ary_push( rbStudentAnswers, rbStudentAnswerComponents );
		}
		rb_ary_push( rbStudents, rbStudentAnswers );
	}
	return rbStudents;
}

/**
 * prepShowImage - Save normalized image to be viewable for modification
 * 
 * @param	filename	Name of the file to normalize
 */
extern "C" VALUE method_prepShowImage(VALUE self, VALUE rubyfilename, VALUE rubyoutname) {
	std::string strfname( StringValueCStr( rubyfilename ) );
	std::string stroutname( StringValueCStr( rubyoutname) );
	ImageReader imr;
	imr.prepShowImage(strfname, stroutname);
	return self;
}
