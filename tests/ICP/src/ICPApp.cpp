#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Capture.h"
#include "cinder/gl/Texture.h"
#include "CinderOpenCv.h"
#include <fstream>
#include <iostream>



using namespace ci;
using namespace ci::app;
using namespace std;

class ICPApp : public AppNative {
public:
    void setup();
    void read_csv(string filename, vector<cv::Mat>& images, vector<int>& labels, char separator);
    //void read_csv(string filename, vector<IplImage>& images, vector<int>& labels, char separator);
    //    void read_csv(string filename, vector<Rectf>& images, vector<int>& labels, char separator);
	void updateFaces( Surface cameraImage );
	void update();
	
	void draw();
    
	Capture			mCapture;
	gl::Texture		mCameraTexture;
	
	cv::CascadeClassifier	mFaceCascade, mEyeCascade;
    cv::Ptr<cv::FaceRecognizer> mFisherFaceRec = cv::createFisherFaceRecognizer();
    
    //  cv::SOMETHING           mPath, mDBLabels;
    string                  mPath;
    vector<cv::Mat>         mDBimgFaces;
    //vector<IplImage>        mIPLimgFaces;
    vector<int>             mDBLabels;
    vector<int>             mDBLabelsTEST;
    //    vector<Rectf>           mDBimgFaces;
    vector<Surface>         mCinderDBimgFaces;
	vector<Rectf>			mFaces, mEyes;
    
    gl::Texture mTexture;
};

void ICPApp::read_csv( string filename, vector<cv::Mat>& images, vector<int>& labels, char separator = ';'){
    ifstream inFile(filename.c_str());
    
    //    std::ifstream file(filename.c_str(), ifstream::in);
    //    if (!file) {
    //        string error_message = "No valid input file was given, please check the given filename.";
    //        CV_Error(CV_StsBadArg, error_message);
    //    }
    string line, path, classlabel;
    while (getline(inFile, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            cout<<path << "\n";
            ci::Surface8u surface( loadImage(path) );
            images.push_back(toOcv(surface));            
            //images.push_back(cvLoadImage((path));
//            images.push_back(cvLoadImage(path, 0);
            //images.push_back(cv::imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

void ICPApp::setup()
{
	mFaceCascade.load( getAssetPath( "haarcascade_frontalface_alt.xml" ).string() );
	mEyeCascade.load( getAssetPath( "haarcascade_eye.xml" ).string() );
    mPath= getAssetPath("ppdtest.csv").string();
    
	mCapture = Capture( 640, 480 );
	mCapture.start();
    
    /*
    ci::Surface8u surface( loadImage("/Users/PpD/Desktop/emotionsrec2/data/emotions/0neutral/amy_adams_neutral.jpg") );
    ci::Surface8u surface2( loadImage("/Users/PpD/Desktop/emotionsrec2/data/emotions/0neutral/angelina_jolie_neutral.jpg") );
    cv::Mat input( toOcv( surface ) );
    mDBimgFaces.push_back(input);
    //mCinderDBimgFaces.push_back(fromOcv(input));
    cv::Mat input2( toOcv( surface2 ) );
    mDBimgFaces.push_back(input2);
    //mCinderDBimgFaces.push_back(fromOcv(input2));
    cv::Mat output;
//    cv::medianBlur( input, output, 11 );
    */
    
    //mTexture = gl::Texture(mCinderDBimgFaces);
    
    //mTexture = gl::Texture( fromOcv( input ) );

    //    cv::Mat output;
//    mTexture = gl::Texture( fromOcv( loadImage("/Users/PpD/Desktop/emotionsrec2/data/emotions/0neutral/amy_adams_neutral.jpg") ) );
    
    
    read_csv(mPath, mDBimgFaces, mDBLabels);
    mFisherFaceRec->train(mDBimgFaces, mDBLabels);
//    mDBLabelsTEST.push_back(0);
//    mDBLabelsTEST.push_back(1);
//    mFisherFaceRec->train(mDBimgFaces, mDBLabelsTEST);
    //mFisherFaceRec->train(mDBimgFaces, mDBLabels);
    
    //cv::Ptr<cv::FaceRecognizer> model = cv::createFisherFaceRecognizer();
    //Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
    //model->train(images, labels);
    
    //    Create a FaceRecognizer:
    //    Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
    //    And here's how to get its name:
    //    std::string name = model->name();
    
    
    
}

void ICPApp::updateFaces( Surface cameraImage )
{
	const int calcScale = 2; // calculate the image at half scale
    
	// create a grayscale copy of the input image
	cv::Mat grayCameraImage( toOcv( cameraImage, CV_8UC1 ) );
    
	// scale it to half size, as dictated by the calcScale constant
	int scaledWidth = cameraImage.getWidth() / calcScale;
	int scaledHeight = cameraImage.getHeight() / calcScale;
	cv::Mat smallImg( scaledHeight, scaledWidth, CV_8UC1 );
	cv::resize( grayCameraImage, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR );
	
	// equalize the histogram
	cv::equalizeHist( smallImg, smallImg );
    
	// clear out the previously deteced faces & eyes
	mFaces.clear();
	mEyes.clear();
    
	// detect the faces and iterate them, appending them to mFaces
	vector<cv::Rect> faces;
	mFaceCascade.detectMultiScale( smallImg, faces );
	for( vector<cv::Rect>::const_iterator faceIter = faces.begin(); faceIter != faces.end(); ++faceIter ) {
		Rectf faceRect( fromOcv( *faceIter ) );
		faceRect *= calcScale;
		mFaces.push_back( faceRect );
		
		// detect eyes within this face and iterate them, appending them to mEyes
		vector<cv::Rect> eyes;
		mEyeCascade.detectMultiScale( smallImg( *faceIter ), eyes );
		for( vector<cv::Rect>::const_iterator eyeIter = eyes.begin(); eyeIter != eyes.end(); ++eyeIter ) {
			Rectf eyeRect( fromOcv( *eyeIter ) );
			eyeRect = eyeRect * calcScale + faceRect.getUpperLeft();
			mEyes.push_back( eyeRect );
		}
	}
}

void ICPApp::update()
{
	if( mCapture.checkNewFrame() ) {
		Surface surface = mCapture.getSurface();
		mCameraTexture = gl::Texture( surface );
		updateFaces( surface );
	}
}

void ICPApp::draw()
{
    gl::clear();
    //gl::draw( mTexture );
    

	if( ! mCameraTexture )
		return;
    
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	
	// draw the webcam image
	gl::color( Color( 1, 1, 1 ) );
	gl::draw( mCameraTexture );
	mCameraTexture.disable();
	
	// draw the faces as transparent yellow rectangles
	gl::color( ColorA( 1, 1, 0, 0.45f ) );
	for( vector<Rectf>::const_iterator faceIter = mFaces.begin(); faceIter != mFaces.end(); ++faceIter )
		gl::drawSolidRect( *faceIter );
	
	// draw the eyes as transparent blue ellipses
	gl::color( ColorA( 0, 0, 1, 0.35f ) );
	for( vector<Rectf>::const_iterator eyeIter = mEyes.begin(); eyeIter != mEyes.end(); ++eyeIter )
		gl::drawSolidCircle( eyeIter->getCenter(), eyeIter->getWidth() / 2 );
     
}

CINDER_APP_NATIVE( ICPApp, RendererGl )
