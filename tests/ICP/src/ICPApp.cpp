#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"
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
    void updateExpressions (Surface cameraImage);
	void update();
    void ColourTheAura(int label);
	void draw();
    
	Capture			mCapture;
	gl::Texture		mCameraTexture;
	
	cv::CascadeClassifier       mExpressionsCascade;
    cv::Ptr<cv::FaceRecognizer> mFisherFaceRec = cv::createFisherFaceRecognizer(); // FACE RECOGNIZER ALGORITHM
    
    string                  mPath;                      // Path for CSV file
    vector<cv::Mat>         mDBimgFaces;                // image vector to train the Face Recognizer
    vector<int>             mDBLabels,mPredictions;     // int vectors for the labels of the Face Recognizer
	vector<Rectf>			mExpressions;
    gl::Texture             faceResized;
    
    // FOR TESTING PURPOSES
    gl::Texture mTexture;
    Surface mSurf;          
    vector<Surface>         mCinderDBimgFaces;
    vector<int>             mDBLabelsTEST;
};

void ICPApp::read_csv( string filename, vector<cv::Mat>& images, vector<int>& labels, char separator = ';'){
    ifstream inFile(filename.c_str());
    string line, path, classlabel;
    while (getline(inFile, line)) {                 // While the file has lines
        stringstream liness(line);                  // Separate the line
        getline(liness, path, separator);           // Put the first part of the line in path
        getline(liness, classlabel);                // Put the second part of the line in classlabel
        if(!path.empty() && !classlabel.empty()) {  // If there are still paths and labels
            //cout<<path << "\n";
            //images.push_back(cv::imread(path, 0));    // imread DOESN'T WORK, it gives compiler errors!!!
            // SINCE the previous line doesn't work...
            ci::Surface8u surface(loadImage(path));       // assign the image to a surface
            cv::Mat graySurface(toOcv(surface, CV_8UC1)); // convert surface to a grayscale Mat variable (OpenCV)
            images.push_back(graySurface);                // put the grayscale image in the img vector
            labels.push_back(atoi(classlabel.c_str()));   // assign the label
        }
    }
}

void ICPApp::setup()
{
    mExpressionsCascade.load(getAssetPath("haarcascade_frontalface_alt.xml").string());
    mPath= getAssetPath("ppdtest.csv").string();
    
	mCapture = Capture( 640, 480 );                 // Camera settings
	mCapture.start();
    
    read_csv(mPath, mDBimgFaces, mDBLabels);        // Read DB of faces for FaceRec algorithm
    mFisherFaceRec->train(mDBimgFaces, mDBLabels);  // Train the Fisher Face Recognizer algorithm
    
    // FOR TESTING PURPOSES
    //    mSurf=(loadImage("/Users/PpD/Desktop/EcA - Pp DanY/MSc ICP/Semester 2/ICP 3/Faces DB Original/hugh_laurie_extra1.jpg"));
    //  mTexture = gl::Texture(mCinderDBimgFaces);
    //  mTexture = gl::Texture( fromOcv( input ) );
    //  cv::Mat output;
    //  mTexture = gl::Texture( fromOcv( loadImage("/Users/PpD/Desktop/emotionsrec2/data/emotions/0neutral/amy_adams_neutral.jpg") ) );    
    //  mDBLabelsTEST.push_back(0);
    //  mDBLabelsTEST.push_back(1);
    //  mFisherFaceRec->train(mDBimgFaces, mDBLabelsTEST);
    //  mFisherFaceRec->train(mDBimgFaces, mDBLabels);
}

void ICPApp::updateExpressions (Surface cameraImage){    
	cv::Mat grayCameraImage( toOcv( cameraImage, CV_8UC1 ) ); // create a grayscale copy of the input image
   	cv::equalizeHist( grayCameraImage, grayCameraImage );	  // equalize the histogram for (just a little) more accuracy
    mExpressions.clear();             // clear out the previously deteced expressions
    mPredictions.clear();
    vector<cv::Rect> expressions;
    
    // Next is to detect the faces and iterate them, appending them to mExpressions
    mExpressionsCascade.detectMultiScale(grayCameraImage, expressions);
    
    // At this point the position of the faces has been calculated!
    // Now it's time to get the faces, make a prediction and annotate it in the video. Awesome or what?
    
    cv::Mat graySq(100,100,CV_8UC1);    // gray square for assigning the proper size of the resized detected faces
    
    for(vector<cv::Rect>::const_iterator expressionIter = expressions.begin(); expressionIter != expressions.end(); ++expressionIter){
        // Get the process face by face (in case there's more than one face in the video frame image)
        Rectf expressionRect(fromOcv(*expressionIter));
        mExpressions.push_back(expressionRect);
        
        cv::Rect face_i (*expressionIter);          // Rect with data (size and position) of the detected face
        cv::Mat face = grayCameraImage(face_i);     // Image containing the detected face
        cv::Mat face_resized;                       // Image for the resized version of the detected face
        cv::resize(face, face_resized, graySq.size(), 1, 1, cv::INTER_CUBIC); // resizes the image
        // cv::resize(face, face_resized, graySq.size(), 0, 0, cv::INTER_LINEAR);

        // Now, perform the... EXPRESSION PREDICTION!!!
        int predicted = mFisherFaceRec->predict(face_resized);
        mPredictions.push_back(predicted);          // put the corresponding label to the corresponding face
        
        // FOR TESTING PURPOSES //
        cout << predicted ;
        // faceResized = (fromOcv(face_resized));
        /////////////////////////
        
    }    
}

void ICPApp::update()
{
	if( mCapture.checkNewFrame() ) {
		Surface surface = mCapture.getSurface();
		mCameraTexture = gl::Texture(surface);
        updateExpressions(surface);
        
//      FOR TESTING PURPOSES
//      mTexture = gl::Texture(mSurf);
//      updateExpressions(mSurf);

	}
}

void ICPApp::ColourTheAura(int label){
    /* Method for the drawing of a transparent rectangle over the face 
     The color depends on the label (the identified expression)
    
     FOR THE TEST DB                    RGB Value
     0neutral       -   Light Blue      173,216,230
     1angry         -   Red             255,0,0
     2contempt      -   Dark Red        139,0,0
     3disgust       -   Purple          128,0,128
     4sad           -   Blue            0,0,255
     5happy         -   Green(Lime)     50,205,50
     6fear          -   Yellow(ish)     255,215,0
     7surprise      -   Orange          255,165,0
     8extra1        -   Gray            169,169,169
     9extra2        -   White           255,255,255
     none           -   Transparent     1,1,1 with 0 alpha
     
     FOR THE PROPER DB
     0neutral       -   Light Blue      173,216,230
     1happy         -   Green(Lime)     50,205,50
     2surprise      -   Orange          255,165,0
     3sad           -   Blue            0,0,255
     4anger         -   Red             255,0,0
     5fear          -   Yellow          255,215,0
     6contempt      -   Dark Red        139,0,0
     7disgust       -   Purple          128,0,128
     8extra1glasses -   Gray            169,169,169
     9extra2other   -   White           255,255,255
     none           -   Transparent     1,1,1 with 0 alpha
     */
    
    if (label == 0)         gl::color( ColorA( (173.f*(1.f/255.f)), (216.f*(1.f/255.f)), (230.f*(1.f/255.f)), 0.45f ) );
    else if (label == 1)    gl::color( ColorA( 1.f, 0.f, 0.f, 0.45f ) );
    else if (label == 2)    gl::color( ColorA( (139.f*(1.f/255.f)), 0.f, 0.f, 0.45f ) );
    else if (label == 3)    gl::color( ColorA( (128.f*(1.f/255.f)), 0.f, (128.f*(1.f/255.f)), 0.45f ) );
    else if (label == 4)    gl::color( ColorA( 0.f, 0.f, 1.f, 0.45f ) );
    else if (label == 5)    gl::color( ColorA( (50.f*(1.f/255.f)), (205.f*(1.f/255.f)), (50.f*(1.f/255.f)), 0.45f ) );
    else if (label == 6)    gl::color( ColorA( 1.f, (215.f*(1.f/255.f)), 0, 0.45f ) );
    else if (label == 7)    gl::color( ColorA( 1.f, (165.f*(1.f/255.f)), 0, 0.45f ) );
    else if (label == 8)    gl::color( ColorA( (169.f*(1.f/255.f)), (169.f*(1.f/255.f)), (169.f*(1.f/255.f)), 0.45f ) );
    else if (label == 9)    gl::color( ColorA( 1.f, 1.f, 1.f, 0.45f ) );
    else{
            gl::color( ColorA( 1.f, 1.f, 1.f, 0.1f ) );
    }
    
    /* The SWITCH version would be...
    switch ( label ) // TEST DB Right Now
    {
        case '0':
            gl::color( ColorA( (173.f*(1.f/255.f)), (216.f*(1.f/255.f)), (230.f*(1.f/255.f)), 0.45f ) );
            break;
        case '1':
            gl::color( ColorA( 1.f, 0.f, 0.f, 0.45f ) );
            break;
        case '2':
            gl::color( ColorA( (139.f*(1.f/255.f)), 0.f, 0.f, 0.45f ) );
            break;
        case '3':
            gl::color( ColorA( (128.f*(1.f/255.f)), 0.f, (128.f*(1.f/255.f)), 0.45f ) );
        case '4':
            gl::color( ColorA( 0.f, 0.f, 1.f, 0.45f ) );
            break;
        case '5':
            gl::color( ColorA( (50.f*(1.f/255.f)), (205.f*(1.f/255.f)), (50.f*(1.f/255.f)), 0.45f ) );
        case '6':
            gl::color( ColorA( 1.f, (215.f*(1.f/255.f)), 0, 0.45f ) );
            break;
        case '7':
            gl::color( ColorA( 1.f, (165.f*(1.f/255.f)), 0, 0.45f ) );
        case '8':
            gl::color( ColorA( (169.f*(1.f/255.f)), (169.f*(1.f/255.f)), (169.f*(1.f/255.f)), 0.45f ) );
            break;
        case '9':
            gl::color( ColorA( 1.f, 1.f, 1.f, 0.45f ) );
            
        default:
            gl::color( ColorA( 1.f, 1.f, 1.f, 0.1f ) );
    }*/
}

void ICPApp::draw()
{
    gl::clear();
	if( ! mCameraTexture )
		return;
	gl::setMatricesWindow( getWindowSize() );
	gl::enableAlphaBlending();
	
	// draw the webcam image
	gl::color( Color( 1, 1, 1 ) );
	gl::draw( mCameraTexture );
	mCameraTexture.disable();
    
////  FOR TESTING PURPOSES  //////
//    gl::draw (mTexture);  
//    gl::draw( faceResized );
//    gl::draw(mTexture);
//////////////////////////////////
    
    // Draw the corresponding coloured squares over the identified expressions
    for( vector<Rectf>::const_iterator expressionIter = mExpressions.begin(); expressionIter != mExpressions.end(); ++expressionIter ){
        ColourTheAura(mPredictions[expressionIter-mExpressions.begin()]);
        gl::drawSolidRect( *expressionIter );
    }
    
}

CINDER_APP_NATIVE( ICPApp, RendererGl )
