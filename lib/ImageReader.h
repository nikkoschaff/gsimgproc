/**
 * ImageReader - Main reading module for GradeSnap
 *
 * @author	Nikko Schaff
 */

#ifndef IMAGEREADER_H_
#define IMAGEREADER_H_

#include <vector>
#include <cmath>
#include <string>
#include <set>
#include <exception>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


class ImageReader {

public: // Methods

	/**
	 * ImageReader - Constructor
	 * @param	numQ	Number of questions on the assignment
	 */
	ImageReader();

	/**
	 * ~ImageReader - Destructor
	 */
	~ImageReader();

	/**
	 * ReadImage - Main function to handle image reading.
	 * If the result is negative, it implies a calibration error
	 * For answers, the floor integer value is the index of the location (-1)
	 *
	 * @param	filename	Name of the file to read
	 * @param	numQuestions Number of questions on the test
	 * @param 	readname 	Boolean to read the name or not
	 * @return	vector< vector< float > >	Answer values in order, last three:
	 *	Name, ambiguousAnswers, ambiguousName (ambiguous letters from name)
	 * 
	 */
	const std::vector< std::vector< float > > 
		readImage( std::string &filename, int numQuestions, bool readname );


	/**
	 * prepShowImage - Save normalized image to be viewable for modification
	 * 
	 * @param	filename	Name of the file to normalize
	 * @param 	outname 	Name of the output file and paras
	 */
	const void prepShowImage( std::string &filename, std::string &outname );

private: // Methods

	/**
	 * SetImage - Set the image given to be the currently-used image, returns
	 *	status of whether it could open it or not
	 *
	 * @param	filename	String of the image filename
	 * @return 	bool 	Whether it could be read or not
	 */
	void setImage( std::string &filename, cv::Mat &examImage );

	/**
	 * FindCalibCorners - Finds and sets the calibration corner points
	 * @returnsbool	True if resultant corners are readable.  False if otherwise
	 */
	void findCalibCornerPoints( cv::Mat &examImage,
	 cv::Point2f &UL, cv::Point2f &UR, cv::Point2f &LL, cv::Point2f &LR );

	/**
	 * OrientImage - Readjust image orientation to be correctly upright
	 */
	void orientImage( cv::Mat &examImage, cv::Point2f &UL, cv::Point2f &UR, 
		cv::Point2f &LL, cv::Point2f &LR,
		float &widthRatio, float &heightRatio );

	/**
	 * ReadAllAnswers - Manages finding answer regions, then reads each
	 */
	void readAllAnswers( cv::Mat &examImage, 
		std::vector< std::vector< float > > &answers, cv::Point2f &UL,
		float &widthRatio, float &heightRatio, int &numQuestions );

	/**
	 * FindAnswerRegions - Finds and stores the answer qbox regions
	 */
	void findAnswerRegions( cv::Mat &examImage,
		std::vector< cv::Rect > &answerRegions, cv::Point2f &UL,
		float &widthRatio, float &heightRatio, int &numQuestions );

	/**
	 * ReadAnswer - Read an answer from a region and return the results
	 * @param	region	QBox
	 * @return	vector<float>	The read results in the answer subregions 
	 */
	std::vector< float > readAnswer( cv::Mat &examImage,
		cv::Rect &region, int refCols[6], float &boxArea,
		float &qHeight );

	/**
	 * ReadName - Read the name from the name boxes
	 */
	void readName( cv::Mat &examImage, cv::Point2f &UL,
		float &widthRatio, float &heightRatio, std::vector< float > &name );

	/**
	 * FindNameLetterRegions - Find and store name letter regions
	 */
	void findNameLetterRegions( cv::Mat &examImage,
		std::vector< cv::Rect > &nameLetterRegions, cv::Point2f &UL,
		float &widthRatio, float &heightRatio  );

	/**
	 * ReadNameLetter - Read and return one name letter
	 * @return	float	The region with the highest concentration of writing.  
	 * 	The location index is the integer in front of the decimal point
	 */
	float readNameLetter( cv::Mat &examImage, 
		cv::Rect &region, int refCols[27], float &boxArea,
		float &qWidth );

	/**
	 * isRectAccurate - Interpret dimensions of a given rotated rectangle
	 *	to see if it's accurately usable
	 *	@param	RotatedRect	The rotated rectangle found from the contour points
	 *	@param	int 	The mode used 
	 *	@return bool	Whether it was readable or not
	 */
	 bool isRectAccurate( cv::RotatedRect &rect, const int &mode );

};
#endif
