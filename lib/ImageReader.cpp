// ImageReader.cpp - Implementation of ImageReader
//
// @author	Nikko Schaff
 

#include "ImageReader.h"

using namespace std;
using namespace cv;

// Max threshold for brightness
static const int MAX_BRIGHTNESS = 20;

static const int NUM_NAME_REGIONS = 17;

// Upper-left on the frame (BASE)
static const cv::Point2f mainUL = Point2f(88, 214);
// Upper-right on the frame (BASE)
static const cv::Point2f mainUR = Point2f(2436, 214);
// Lower-left on the frame (BASE)
static const cv::Point2f mainLL = Point2f(88, 3214);
// Lower-right on the frame (BASE)
static const cv::Point2f mainLR = Point2f(2436, 3214);

// Minimum threshold of area for the frame
static const int MAIN_FRAME_MIN_THRESH = 1000000;

// Width of the base question box
static const float MAIN_QBOX_WIDTH = 225.0f;
// Height of the base question box
static const float MAIN_QBOX_HEIGHT = 68.0f;

// Offset by X from UL on base image
static const float MAIN_START_X_OFFSET = 154.0f;
// Offset by Y from UL on base image
static const float MAIN_START_Y_OFFSET = 445.0f;
// Offset by X from each QBox on base image
static const float MAIN_QBOX_X_OFFSET = 304.0f;
// Offset by Y from each QBOX on base image
static const float MAIN_QBOX_Y_OFFSET = 86.5f;

// Letter box details
	
// Width of the name box
static const float MAIN_NBOX_WIDTH = 40;
// Height of the name box
static const float MAIN_NBOX_HEIGHT = 2262;

// Offset by X from UL on base image for Nbox
static const float MAIN_START_NBOX_X_OFFSET = 1404;
// Offset by Y from UL on base image for Nbox
static const float MAIN_START_NBOX_Y_OFFSET = 433;

// Offset by X from each NBOX on base image
static const float MAIN_NBOX_X_OFFSET = 45.3f;
// Offset by Y from each NBOX on base image
static const float MAIN_NBOX_Y_OFFSET = 47;

// Offset by X between first and MI on base image
static const float MAIN_NBOX_FIRST2MI_X_OFFSET = 80;
// Offest by X between MI and Last on base image
static const float MAIN_NBOX_MI2LAST_X_OFFSET = 84;


//Macros for isRectAccurate mode
static const int CALIB_RECT = 0;
static const int ROTATION_BOX = 1;

// Ratio values for rect accuracy
static const float ACCURACY_MODIFIER = 1.05f;
static const float CALIB_RATIO = .782594f;
static const float CALIB_RATIO_UPPER = .8217237f;
static const float CALIB_RATIO_LOWER = .745327619f;
static const float CALIB_RATIO_INV = 1.2778f;
static const float CALIB_RATIO_INV_UPPER = 1.34169f;
static const float CALIB_RATIO_INV_LOWER = 1.21695238f;
static const float ROTATED_RATIO = 1; 
static const float ROTATED_RATIO_UPPER = 1.05f;
static const float ROTATED_RATIO_LOWER = .95f;


/**
	* ImageReader - Constructor
	* @param	numQ	Number of questions on the assignment
	*/
ImageReader::ImageReader(){
}

/**
	* ~ImageReader - Destructor
	*/
ImageReader::~ImageReader() {}

/**
 * ReadImage - Main function to handle image reading.
 * If the result is negative, it implies a calibration error
 * For answers, the floor integer value is the index of the location (-1)
 *
 * @param	filename	Name of the file to read
 * @param	numQuestions Number of questions on the test
 * @param 	readname 	Boolean to read the name or not
 * @return	vector< vector< float > >	Answer values in order, last three:
 * 
 */	
const std::vector< std::vector< float > > ImageReader::readImage( std::string &filename,
	int numQuestions, bool readname ) {
	// Image of the assignment
	cv::Mat examImage;
	// Ratio of exam:base image width
	float widthRatio = 0;
	// Ratio of exam:base image height
	float heightRatio = 0;
	// Array of answers
	std::vector< std::vector< float > > answers(numQuestions);
	// name letters
	std::vector< float > name( 17 );
	// Upper-left on the frame
	cv::Point2f UL;
	// Upper-right on the frame
	cv::Point2f UR;
	// Lower-left on the frame
	cv::Point2f LL;
	// Lower-right on the frame
	cv::Point2f LR;

	// Set the image
	try {
		setImage( filename, examImage );
	} catch (...) {
		vector< float > oops;
		oops.push_back( -1.0f );
		answers.push_back( oops );
		return answers;
	}

	// Compute the calibration corner points
	// Checks to see if image was readable or not.  If not, adds "ERR" to ans
	// And returns
	try {
		findCalibCornerPoints( examImage, UL, UR, LL, LR );
	} catch (...) {
		vector< float > oops;
		oops.push_back( -2.0f );
		answers.push_back( oops );
		return answers;
	}

	try {
		// Orient the image
		orientImage( examImage, UL, UR, LL, LR, widthRatio, heightRatio );
	} catch (...) {
		vector< float > oops;
		oops.push_back( -3.0f );
		answers.push_back( oops );
		return answers;
	}

	try { 
		// Threshold the image so only filled/dark spaces remain for reading
		adaptiveThreshold( examImage, examImage, 255, 
			ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 51, 7 );	

		// Read answers
		readAllAnswers( examImage, answers, 
			UL, widthRatio, heightRatio, numQuestions );
		// If name is to be read, read and add the name
		// Otherwise, add a blank space (for consistency)
		if( readname ) {
			readName( examImage, UL, widthRatio, heightRatio, name );
		}
		answers.push_back( name );
	} catch (...) {
        vector< float > oops;
        oops.push_back( -4.0f );
        answers.push_back( oops );
        return answers;
	}	
	return answers;
}

/**
 * prepShowImage - Save normalized image to be viewable for modification
 * 
 * @param	filename	Name of the file to normalize
 * @param 	outname 	Name of the output file and paras
 */
const void ImageReader::prepShowImage( std::string &filename, std::string &outname ) {
	// Image of the assignment
	cv::Mat examImage;
	// Ratio of exam:base image width
	float widthRatio = 0;
	// Ratio of exam:base image height
	float heightRatio = 0;
	// Upper-left on the frame
	cv::Point2f UL;
	// Upper-right on the frame
	cv::Point2f UR;
	// Lower-left on the frame
	cv::Point2f LL;
	// Lower-right on the frame
	cv::Point2f LR;

	// Set the image
	try {
		setImage( filename, examImage );
		findCalibCornerPoints( examImage, UL, UR, LL, LR );
		orientImage( examImage, UL, UR, LL, LR, widthRatio, heightRatio );
		vector<int> compression_params;
		compression_params.push_back( CV_IMWRITE_JPEG_QUALITY );
        imwrite(outname, examImage, compression_params);
    } catch (...) {
        return;
    }
}

/**
 * SetImage - Set the image given to be the currently-used image, returns
 *	status of whether it could open it or not
 *
 * @param	filename	String of the image filename
 * @return 	bool 	Whether it could be read or not
 */
void ImageReader::setImage( std::string &filename, Mat &examImage ) {
	examImage = imread( filename, 0 );
	if (examImage.data == NULL) {
		throw new Exception;
	}
}

/**
 * FindCalibCorners - Finds and sets the calibration corner points
 * @returnsbool	True if resultant corners are readable.  False if otherwise
 */
void ImageReader::findCalibCornerPoints( Mat &examImage, cv::Point2f &UL, cv::Point2f &UR, 
	cv::Point2f &LL, cv::Point2f &LR ) {
	// Copy of the image, as the functions drastically modify it
	Mat examCopy = examImage.clone();
	// Calib box UL point
	cv::Point boxUL;

	// Dilates the exam image to reduce noise
	dilate( examCopy, examCopy, Mat(), Point(-1,-1), 2 );
	//-- 2: Smooth, also reduces noise
	GaussianBlur( examCopy, examCopy, Size( 3, 3 ), 0, 0 );
	// Get the image to b/w basics
	adaptiveThreshold( examCopy, examCopy, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 10 );
	// Dilates the exam image to reduce noise
	erode( examCopy, examCopy, Mat(), Point(-1,-1), 1 );
	//-- 3a: Detect edges from the (now extracted) frame by Canny method
	Canny( examCopy, examCopy, 150, 250 );
	//-- 4: Find contours to establish the interesting marks
	vector< vector< Point > > contours;
	findContours( examCopy, contours, RETR_LIST, CHAIN_APPROX_SIMPLE );

	//-- 7: Finds correct contours for calib corners and sends
	//		rectangle representation to the corner rectangle vector	
	float area = 0;
	float calibRectArea = 0;
	float calibRectIndex = 0;
	float boxRectArea = 0;
	float boxRectIndex = 0;
	RotatedRect minArRect;
	bool crChosen = false;
	bool brChosen = false;
	int contoursSize = int(contours.size());
	Point2f pts[4];
	for ( int i = 0; i < contoursSize; i++ ) {
		minArRect = minAreaRect( contours[i] );
		area = minArRect.size.area();
		// If high enough for large rect consideration
		if( area > 100000 && area > calibRectArea
			&& isRectAccurate( minArRect, CALIB_RECT ) ) {
			calibRectArea = area;
			calibRectIndex = i;
			crChosen = true;
		// If high enough for box consideration
		} else if ( area < 100000 && area > 200 && area > boxRectArea
			&& isRectAccurate( minArRect, ROTATION_BOX ) ) {
			boxRectArea = area;
			boxRectIndex = i;
			brChosen = true;
		}
	}

	// Readability checking
	if( !brChosen || !crChosen ) {
		throw new Exception;
	}
	// Assign calibration points to be at the center of the image.
	Rect box = minAreaRect( contours[boxRectIndex] ).boundingRect();
	boxUL = box.tl();
	minAreaRect( contours[calibRectIndex] ).points( pts );
	UL = pts[0];
	UR = pts[1];
	LL = pts[2];
	LR = pts[3];

	// Region from the exam image
	vector< Point2f > eRectPoints(4);
	eRectPoints[0] = UL;
	eRectPoints[1] = UR;
	eRectPoints[2] = LL;
	eRectPoints[3] = LR;
	// ---- Handling to fix rotation ---- 
	// Points are always found in clockwise rotation from the first point.
	// All that matters is the correct shifts are made
	float distanceFromBox = 0.0f;
	float minDistance = INT_MAX;
	float minIndex = 0.0f;
	for( int i = 0; i < 4; i++ ) {
		distanceFromBox = ( abs( eRectPoints[i].x - boxUL.x ) 
			+ abs( eRectPoints[i].y - boxUL.y ) );
		if( distanceFromBox < minDistance ) {
			minDistance = distanceFromBox;
			minIndex = i;
		}
	}
	if ( minIndex == 0 ) {
		// If first found is closest to the box
		UL = eRectPoints[0];
		UR = eRectPoints[1];
		LL = eRectPoints[3]; //Switched because clockwise in contour-finding
		LR = eRectPoints[2]; //Same as above
	}
	if ( minIndex == 1 ) {
		// If second found is closest to the box
		UL = eRectPoints[1];
		UR = eRectPoints[2];
		LL = eRectPoints[0];
		LR = eRectPoints[3];
	} else if ( minIndex == 2 ) {
		// If third found is closest to the box
		UL = eRectPoints[2];
		UR = eRectPoints[3];
		LL = eRectPoints[1];
		LR = eRectPoints[0];
	} else if ( minIndex == 3 ) {
		// If fourth found is closest to the box
		UL = eRectPoints[3];
		UR = eRectPoints[0];
		LL = eRectPoints[2];
		LR = eRectPoints[1];
	}
}

/**
 * OrientImage - Readjust image orientation to be correctly upright
 */
void ImageReader::orientImage( cv::Mat &examImage, cv::Point2f &UL, cv::Point2f &UR, 
	cv::Point2f &LL, cv::Point2f &LR, float &widthRatio, float &heightRatio ) {
	// Set up lengths and widths
	float vLength = sqrt( pow( abs(LL.x - UL.x), 2 ) + pow( abs(LL.y - UL.y), 2 ) );
	float hLength = sqrt( pow( abs(UR.x - UL.x), 2 ) + pow( abs(UR.y - UL.y), 2 ) );
	// handle rotation issue (if on side - vertical length "less" than horizontal)
	if( vLength < hLength ) {
		float vTemp = vLength;
		vLength = hLength;
		hLength = vTemp;
	} 
	// Locates the ROI offset for its ul
	Point2f srcQuad[4], dstQuad[4];
	vector< Point2f > eRectPoints(4);
	eRectPoints[0] = UL;
	eRectPoints[1] = UR;
	eRectPoints[2] = LL;
	eRectPoints[3] = LR;
	Rect examRegion = minAreaRect( eRectPoints ).boundingRect();
	Size size;
	Point ofs;
	Mat examCopy = examImage( examRegion );
	examCopy.locateROI( size, ofs );
	// Set up points for the "source" array
	srcQuad[0] = Point2f( ofs.x + abs(UL.x - ofs.x), abs( UL.y - ofs.y ) );
	srcQuad[1] = Point2f( srcQuad[0].x + hLength, srcQuad[0].y );
	srcQuad[2] = Point2f( srcQuad[0].x, srcQuad[0].y + vLength );
	srcQuad[3] = Point2f( srcQuad[0].x + hLength, srcQuad[0].y + vLength );
	// Set up points for the "destination" array
	dstQuad[ 0 ] = UL;
	dstQuad[ 1 ] = UR;
	dstQuad[ 2 ] = LL;
	dstQuad[ 3 ] = LR;
	// Calc perspective transform, prepare the clone, & warp
	Mat warp_matrix = getPerspectiveTransform( srcQuad, dstQuad );
	warpPerspective( examImage.clone(), examImage, warp_matrix,
	 Size(examImage.cols * 1.5f, examImage.rows * 1.5f), WARP_INVERSE_MAP );
	// Resize examImage to only have coordinate values
	Rect rect( srcQuad[0], srcQuad[3] );
	Mat fittedImage = examImage( rect );
	examImage = fittedImage;
	// Recalculates size ratios
	widthRatio = examImage.cols / (mainUR.x - mainUL.x);
	heightRatio = examImage.rows / (mainLL.y - mainUL.y);
	// Reassign points
	UL = Point2f( 0, 0 );
	UR = Point2f( hLength, 0 );
	LL = Point2f( 0, vLength );
	LR = Point2f( hLength, vLength );
}

/**
 * ReadAllAnswers - Manages finding answer regions, then reads each
 */
void ImageReader::readAllAnswers( cv::Mat &examImage, 
	std::vector< std::vector< float > > &answers, cv::Point2f &UL,
	float &widthRatio, float &heightRatio, int &numQuestions ) {

	// QBox regions
	std::vector< cv::Rect > answerRegions(numQuestions);	
	// Locate the answer box regions
	findAnswerRegions( examImage, answerRegions, UL,
		widthRatio, heightRatio, numQuestions );

	int refCols[6];
	float distWidth = answerRegions[0].width/5.0f;
	float qHeight = answerRegions[0].height;
	for( int a = 0; a < 6; a++ ) refCols[a] = distWidth * a;
	float boxArea = distWidth * answerRegions[0].height;

	for( int i = 0; i < numQuestions; i++ ) {
		answers[i] =  readAnswer( examImage, answerRegions[i],
			refCols, boxArea, qHeight );
	}
}

/**
 * FindAnswerRegions - Finds and stores the answer qbox regions
 */
void ImageReader::findAnswerRegions( cv::Mat &examImage,
	std::vector< cv::Rect > &answerRegions, cv::Point2f &UL,
	float &widthRatio, float &heightRatio, int &numQuestions ) {
	//1st qbox coordinates, will be set as points for Q1's bounding box
	float firstQBoxX( UL.x + ( MAIN_START_X_OFFSET * widthRatio ) );
	float firstQBoxY( UL.y  + ( MAIN_START_Y_OFFSET * heightRatio ) ); 
	//Relative coordinates, used for each Qs bounding box
	float currentX( firstQBoxX );
	float currentY( firstQBoxY );
	// Relative offset by X for each qbox
	float relxoff = MAIN_QBOX_X_OFFSET * widthRatio;
	// Relative offset by Y for each qbox
	float relyoff = MAIN_QBOX_Y_OFFSET * heightRatio;
	// Width of the qbox
	float qboxWidth( MAIN_QBOX_WIDTH * widthRatio );
	// Height of the qbox
	float qboxHeight( MAIN_QBOX_HEIGHT * heightRatio );

	int turns[] = {24,49,74};
	set<int> turnSet(turns,turns+3);

	//Find the ROIs located at each position
	for ( int numQ = 0; numQ < numQuestions; numQ++ ) {
		// Create and store ROI
		Rect answerRegion( int( currentX ), int( currentY ),
		 qboxWidth, qboxHeight );
		
		/*
		rectangle( examImage, Point( currentX, currentY ), Point( currentX + qboxWidth,
			currentY + qboxHeight ), CV_RGB(100,100,100), 1 );
		*/

		// Stores the answer region ROI into the vector
		answerRegions[numQ] = answerRegion;
		
		// If at the end of a column, move to the upper-left of the next column
		if ( turnSet.count(numQ) == 1 ) { 
			currentX += relxoff;
			currentY = firstQBoxY;
		} else {
			// Otherwise, just move to the next question down
			currentY += relyoff;	
		}
	}
}


/**
 * ReadAnswer - Read an answer from a region and return the results
 * @param	region	QBox
 * @return	vector<float>	The read results in the answer subregions 
 */
std::vector< float > ImageReader::readAnswer( cv::Mat &examImage, 
	cv::Rect &region, int refCols[6], float &boxArea, float &qHeight ) {
	//Set up a projection for each of the five possible answer choices
	std::vector< float > answer( 5 );
	//Set the ROI for the image copy
	Mat answerROI = examImage( region );
	// Number of dark spots in an image region
	float darkCount;
	// Scalar value of the pixel at chosen location
	Scalar intensity;
	// Start column value for a subregion
	int startCol;
	// End column value for a subregion
	int endCol;

	//For each subdivision
	for( int a = 0; a < 5; a++ ) {
		// Resets the darkCount to zero and recalculates column locations
		darkCount = 0;
		startCol = refCols[a];
		endCol = refCols[a+1];
		// Iterates through each column (by width)
		for( int col = startCol; col < endCol; col++ ) {
			// Iterates through each row (by height)
			for( int row = 0; row < qHeight; row++ ) {
				// Sets the intensity as the value of the pixel at that location
				darkCount += Scalar(answerROI.at<uchar>(row, col)).val[0];
			}
		}
		answer[a] = ( (darkCount/255)/boxArea );
	}
	return answer;
}

/**
 * ReadName - Read the name from the name boxes
 */
void ImageReader::readName( cv::Mat &examImage, cv::Point2f &UL,
	float &widthRatio, float &heightRatio, std::vector< float > &name ) {
	// Name letter regions
	std::vector< cv::Rect > nameLetterRegions(NUM_NAME_REGIONS);	
	findNameLetterRegions( examImage, nameLetterRegions,
		UL, widthRatio, heightRatio );

	int refCols[27];
	float distHeight = nameLetterRegions[0].height/26.0f;
	for( int a = 0; a < 27; a++ ) refCols[a] = distHeight * a;
	float boxArea = nameLetterRegions[0].width * distHeight;
	float qWidth = nameLetterRegions[0].width;

	for(  int i = 0; i < NUM_NAME_REGIONS; i++ ) {
		name[i] = readNameLetter( examImage, nameLetterRegions[i],
			refCols, boxArea, qWidth );
	}
}

/**
 * FindNameLetterRegions - Find and store name letter regions
 */
void ImageReader::findNameLetterRegions( cv::Mat &examImage,
	std::vector< cv::Rect > &nameLetterRegions, cv::Point2f &UL,
	float &widthRatio, float &heightRatio  ) {
	//1st Nbox coordinates, will be set as points for N1's bounding box
	float firstNBoxX( UL.x + ( MAIN_START_NBOX_X_OFFSET * widthRatio ) );
	float firstNBoxY( UL.y + ( MAIN_START_NBOX_Y_OFFSET * heightRatio ) ); 
	//Relative coordinates, used for each Ns bounding box
	float currentX( firstNBoxX );
	float currentY( firstNBoxY );
	// Relative offset by X for each nbox
	float relxoff = MAIN_NBOX_X_OFFSET * widthRatio;
	// Relative offset by Y for each nbox
	float relyoff = MAIN_NBOX_Y_OFFSET * heightRatio;
	// Width of the qbox
	float nboxWidth( MAIN_NBOX_WIDTH * widthRatio );
	// Height of the qbox
	float nboxHeight( MAIN_NBOX_HEIGHT * heightRatio );
	// adjusted-to-mi offset
	float adjToMiOffset = MAIN_NBOX_FIRST2MI_X_OFFSET * widthRatio;
	// adjusted-from-mi offset
	float adjFromMiOffset = MAIN_NBOX_MI2LAST_X_OFFSET * widthRatio;

	//Find the ROIs located at each position
	for ( int i = 0; i < NUM_NAME_REGIONS; i++ ) {
		// Create and store ROI
		Rect nameLetterRegion( int( currentX ), int( currentY ),
		 nboxWidth, nboxHeight );

		/*
		rectangle( examImage, Point( currentX, currentY ), Point( currentX + nboxWidth,
			currentY + nboxHeight ), CV_RGB(100,100,100), 1 );
		*/	

		// Stores the answer region ROI into the vector
		nameLetterRegions[i] = nameLetterRegion;

		// Handle middle initial (which has different spacing issues)
		if( i == 7 ) {
			currentX += adjToMiOffset;
		} else if ( i == 8 ) {
			currentX += adjFromMiOffset;
		} else {
			// Moves to the next box location
			currentX += relxoff;
		}
	}
}

/**
 * ReadNameLetter - Read and return one name letter
 * @return	float	The region with the highest concentration of writing.  
 * 	The location index is the integer in front of the decimal point
 */
float ImageReader::readNameLetter( cv::Mat &examImage,
	cv::Rect &region, int refCols[27], float &boxArea,
	float &qWidth ) {
	//Set up a projection for each of the five possible letter choices
	float letter;
	//Set the ROI for the image copy
	Mat letterROI = examImage( region );
	// Number of dark spots in an image region
	float darkCount;
	// Scalar value of the pixel at chosen location
	Scalar intensity;
	// Start column value for a subregion
	int startRow;
	// End column value for a subregion
	int endRow;

	float highestCount = 0; 
	int highestIndex = 0;

	//For each subdivision
	for( int a = 0; a < 26; a++ ) {
		// Resets the darkCount to zero and recalculates column locations
		darkCount = 0;
		startRow = refCols[a];
		endRow = refCols[a+1];
		// Iterates through each column (by width)
		for( int col = 0; col < qWidth; col++ ) {
			// Iterates through each row (by height)
			for( int row = startRow; row < endRow; row++ ) {
				// Sets the intensity as the value of the pixel at that location
				darkCount += Scalar(letterROI.at<uchar>(row, col)).val[0];
			}
		}
		// Checks to see if it accurately corresponds with an answer region
		if( darkCount > highestCount ) {
			highestCount = darkCount;
			highestIndex = a;
		}
	}
	return ((highestCount/255)/boxArea + highestIndex);
}

/**
 * isRectAccurate - Interpret dimensions of a given rotated rectangle
 *	to see if it's accurately usable
 *	@param	RotatedRect	The rotated rectangle found from the contour points
 *	@param	int 	The mode used 
 *	@return bool	Whether it was readable or not
 */
bool ImageReader::isRectAccurate( cv::RotatedRect &rect, const int &mode ) {
 	float ratio = rect.size.height / rect.size.width;

 	// If Calib_Rect, then check to see if ratios match base ratios - either long/wide or 
 	//	wide/long to ensure fairness - within scope of accuracy modifier.
 	if ( mode == CALIB_RECT ) {
 		if( (CALIB_RATIO_UPPER > ratio && CALIB_RATIO_LOWER < ratio)
 			|| ( CALIB_RATIO_INV_UPPER > ratio && CALIB_RATIO_INV_LOWER < ratio ) ) {				
 			return true;
 		}
 	// If Rotation_box, check to see if ratios are roughly 1, within scopre of
 	//	Accuracy modifier.
 	} else if ( mode == ROTATION_BOX ) {
 		if( ROTATED_RATIO_UPPER > ratio && ROTATED_RATIO_LOWER < ratio ) {
 			return true;
 		}
 	}
 	return false;
 }
