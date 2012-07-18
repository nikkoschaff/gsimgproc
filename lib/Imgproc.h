// Include the Ruby headers and goodies
#include "ruby.h"

#ifdef __cplusplus
extern "C" {
#endif

// Defining a space for information and references about the module
// to be stored internally
extern VALUE irm;

// Prototype for the initialization method - Ruby calls this, not you (.new)
void Init_Imgproc();

// Initialization for "class" itself
VALUE method_init(VALUE self);

// Reads the filenames with the specified number of questions and
// 	boolean to read the name or not
VALUE method_readFiles(VALUE self, VALUE rubyfilenames,
 VALUE rubynumQ, VALUE rubyReadname);

// Normalizes and saves image for further viewing
VALUE method_prepShowImage(VALUE self, VALUE rubyfilename, VALUE rubyoutname);

#ifdef __cplusplus
}
#endif
