#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"
#include "cinder/Capture.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "CinderOpenCv.h"
#include <fstream>
#include <iostream>

using namespace ci;
using namespace ci::app;
using namespace std;

static const int WIDTH = 640, HEIGHT = 480;

class ICPApp2 : public AppNative {
public:
    void setup();
   	void keyDown( KeyEvent event );
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
	vector<Rectf>			mExpressions;               // rect vectors to save the position of the faces
    bool                    extCamExists;               // flag to check if there's an external camera.
    Capture::DeviceRef      extCam;                     // variable to save external camera device if there is one
    bool                    circEff, rectEff;           // flag variables for effects
    bool                    aurEff, sprSlrpEff;         // flag variables for effects
    uint                    frameCount;
    
    // FOR TESTING PURPOSES
    gl::Texture mTexture;
    gl::Texture             faceResized;
    Surface mSurf;
    vector<Surface>         mCinderDBimgFaces;
    vector<int>             mDBLabelsTEST;
};

void ICPApp2::read_csv( string filename, vector<cv::Mat>& images, vector<int>& labels, char separator = ';'){
    ifstream inFile(filename.c_str());
    string line, path, classlabel;
    while (getline(inFile, line)) {                 // While the file has lines
        stringstream liness(line);                  // Separate the line
        getline(liness, path, separator);           // Put the first part of the line in path
        getline(liness, classlabel);                // Put the second part of the line in classlabel
        if(!path.empty() && !classlabel.empty()) {  // If there are still paths and labels
            //images.push_back(cv::imread(path, 0));    // imread DOESN'T WORK, it gives compiler errors!!!
            // SINCE the previous line doesn't work...
            ci::Surface8u surface(loadImage(path));       // assign the image to a surface
            cv::Mat graySurface(toOcv(surface, CV_8UC1)); // convert surface to a grayscale Mat variable (OpenCV)
            images.push_back(graySurface);                // put the grayscale image in the img vector
            labels.push_back(atoi(classlabel.c_str()));   // assign the label
        }
    }
}

void ICPApp2::setup()
{
    mExpressionsCascade.load(getAssetPath("haarcascade_frontalface_alt.xml").string());
    //mPath= getAssetPath("emotions.csv").string();
    //FOR TEST DB
    mPath= getAssetPath("ppdtest.csv").string();
    circEff = true;
    sprSlrpEff = false;
    aurEff = false;
    rectEff = false;
    
    
    ///////////// SETUP CAMERA SETTINGS //////////////
    try{                                            // Try to initialize a camera.
        try{                                        // Try to find an external camera.
            extCam = Capture::getDevices().at(1);   // Get external camera device info.
            extCamExists = true;                    // Set flag to true
        }catch(...){                                // Catch if no external camera...
            extCamExists = false;                   // ... and set flag to false
        }
        if( extCamExists ) {                        // If there is an external camera, use it
            mCapture = Capture( WIDTH, HEIGHT, extCam );
            mCapture.start();
        }else{                                      // Otherwise use the integrated camera
            mCapture = Capture( WIDTH, HEIGHT );
            mCapture.start();
        }
    }catch( CaptureExc & ) {                        // Catch if no camera was found or there was a problem initializing it
        console() << "Failed to initialize any camera: " << endl;
    }
    //////////////////////////////////////////////////
    
    read_csv(mPath, mDBimgFaces, mDBLabels);        // Read DB of faces for FaceRec algorithm
    mFisherFaceRec->train(mDBimgFaces, mDBLabels);  // Train the Fisher Face Recognizer algorithm
    
    // FOR TESTING PURPOSES
        frameCount=0;
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

void ICPApp2::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )                    // enter OR leave full-screen mode
		setFullScreen( ! isFullScreen() );
    if( event.getCode() == KeyEvent::KEY_ESCAPE ){  // leave full-screen mode
        if (isFullScreen())
            setFullScreen(!isFullScreen());
    }
    
    if( event.getChar() == 'a' )                    // activate OR deactivate aurEff
		aurEff = !aurEff;
    if( event.getChar() == 'c' )                    // activate OR deactivate circEff
		circEff = !circEff;
    if( event.getChar() == 'r' )                    // activate OR deactivate rectEff
		rectEff = !rectEff;
    if( event.getChar() == 's' )                    // activate OR deactivate sprSlrpEff
		sprSlrpEff = !sprSlrpEff;
    
}

void ICPApp2::updateExpressions (Surface cameraImage){
	cv::Mat grayCameraImage( toOcv( cameraImage, CV_8UC1 ) ); // create a grayscale copy of the input image
   	cv::equalizeHist( grayCameraImage, grayCameraImage );	  // equalize the histogram for (just a little) more accuracy
    mExpressions.clear();             // clear out the previously deteced expressions
    mPredictions.clear();
    vector<cv::Rect> expressions;
    
    // Next is to detect the faces and iterate them, appending them to mExpressions
    mExpressionsCascade.detectMultiScale(grayCameraImage, expressions);
    
    // At this point the position of the faces has been calculated!
    // Now it's time to get the faces, make a prediction and annotate it in the video.
    
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
        
        // Now, perform the EXPRESSION PREDICTION
        int predicted = mFisherFaceRec->predict(face_resized);
        mPredictions.push_back(predicted);          // put the corresponding label to the corresponding face
        
        // FOR TESTING PURPOSES //
        // cout << predicted ;
        // faceResized = (fromOcv(face_resized));
        /////////////////////////
        
    }
}

void ICPApp2::update()
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

void ICPApp2::ColourTheAura(int label){
    /* Method for the drawing of a transparent rectangle over the face
     The color depends on the label (the identified expression)
     
     FOR THE TEST DB                    RGB Value
     0neutral       -   White           255,255,255
     1angry         -   Red             255,0,0
     2contempt      -   Green(Lime)     50,205,50
     3disgust       -   Brown(Saddle)   139,69,19
     4sad           -   Blue            0,0,255
     5happy         -   Yellow(gold)    255,215,0
     6fear          -   Black           0,0,0
     7surprise      -   Orange          255,165,0
     8extra1        -   Purple          128,0,128
     9extra2        -   Pink            238,15,151
     none           -   Transparent     255,255,255 with 0.1 alpha   ***Bug only. Should never happen***
    */
    
    if (label == 0)         gl::color( ColorA( 1.f, 1.f, 1.f, 0.45f ) );
    else if (label == 1)    gl::color( ColorA( 1.f, 0.f, 0.f, 0.45f ) );
    else if (label == 2)    gl::color( ColorA( (50.f*(1.f/255.f)), (205.f*(1.f/255.f)), (50.f*(1.f/255.f)), 0.45f ) );
    else if (label == 3)    gl::color( ColorA( (139.f*(1.f/255.f)), (69.f*(1.f/255.f)), (19.f*(1.f/255.f)), 0.45f ) );
    else if (label == 4)    gl::color( ColorA( 0.f, 0.f, 1.f, 0.45f ) );
    else if (label == 5)    gl::color( ColorA( 1.f, (215.f*(1.f/255.f)), 0, 0.45f ) );
    else if (label == 6)    gl::color( ColorA( 0.f, 0.f, 0.f, 0.45f ) );
    else if (label == 7)    gl::color( ColorA( 1.f, (165.f*(1.f/255.f)), 0, 0.45f ) );
    else if (label == 8)    gl::color( ColorA( (128.f*(1.f/255.f)), 0.f, (128.f*(1.f/255.f)), 0.45f ) );
    else if (label == 9)    gl::color( ColorA( (238.f*(1.f/255.f)), (15.f*(1.f/255.f)), (151.f*(1.f/255.f)), 0.45f ) );
    else{
        gl::color( ColorA( 1.f, 1.f, 1.f, 0.1f ) );
    }
    
    /*
    FOR THE PROJECT DB
    0neutral       -   White           255,255,255
    1happy         -   Yellow(gold)    255,215,0
    2surprise      -   Orange          255,165,0
    3sad           -   Blue            0,0,255
    4anger         -   Red             255,0,0
    5fear          -   Black           0,0,0
    6contempt      -   Green(Lime)     50,205,50
    7disgust       -   Brown(Saddle)   139,69,19
    none           -   Transparent     255,255,255 with 0.1 alpha   ***Bug only. Should never happen***
    */
    
    /*
    if (label == 0)         gl::color( ColorA( 1.f, 1.f, 1.f, 0.45f ) );
    else if (label == 1)    gl::color( ColorA( 1.f, (215.f*(1.f/255.f)), 0.f, 0.45f ) );
    else if (label == 2)    gl::color( ColorA( 1.f, (165.f*(1.f/255.f)), 0, 0.45f ) );
    else if (label == 3)    gl::color( ColorA( 0.f, 0.f, 1.f, 0.45f ) );
    else if (label == 4)    gl::color( ColorA( 1.f, 0.f, 0.f, 0.45f ) );
    else if (label == 5)    gl::color( ColorA( 0.f, 0.f, 0.f, 0.45f ) );
    else if (label == 6)    gl::color( ColorA( (50.f*(1.f/255.f)), (205.f*(1.f/255.f)), (50.f*(1.f/255.f)), 0.45f ) );
    else if (label == 7)    gl::color( ColorA( (139.f*(1.f/255.f)), (69.f*(1.f/255.f)), (19.f*(1.f/255.f)), 0.45f ) );
    else{
        gl::color( ColorA( 1.f, 1.f, 1.f, 0.1f ) );
    }*/
}


void ICPApp2::draw()
{
    gl::clear();
	if( ! mCameraTexture )
		return;
    
    ////  FOR TESTING PURPOSES  //////
    //    gl::draw (mTexture);
    //    gl::draw( faceResized );
    //    gl::draw(mTexture);
    //////////////////////////////////
    
    // get window size
    int w = getWindowWidth();
    int h = getWindowHeight();
    //int h = w / ( WIDTH / (float)HEIGHT ); // h=1080 in fullscreen
    
    // determine scale
    float s = std::min<float>( ((float) w / WIDTH), ((float) h / HEIGHT) ); //s=2.25 with ratio loss, s=1.875 if no ratio loss
    // cout << "the value of s="<<s<<"\n";
    
    // center content
    int ws = (int) (WIDTH * s + 0.5f);
    int hs = (int) (HEIGHT * s + 0.5f); //hs = 1080 in full screen
    int x = (w - ws) / 2;
    int y = (h - hs) / 2;
    
    // move origin and scale content
    gl::pushModelView();
    gl::setMatricesWindow( getWindowSize() );
    gl::enableAlphaBlending();
    
    gl::translate(x, y, 0.0f);
    gl::scale(s, s, s);
    
    gl::color( Color::white() );
    gl::draw( mCameraTexture );
    mCameraTexture.disable();
    
    // Draw the corresponding effects over the identified expressions
    for( vector<Rectf>::const_iterator expressionIter = mExpressions.begin(); expressionIter != mExpressions.end(); ++expressionIter ){
        ColourTheAura(mPredictions[expressionIter-mExpressions.begin()]); // Set colour
        // Draw a circle around the face so that there's always at least this effect
        gl::enableWireframe();
        gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.7f);
        gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.705f);
        gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.7101f);
        gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.714f);
        gl::disableWireframe();
        // Check what effects are activated and produce the effect(s)
        if(circEff){ // Activated by default
            if(sprSlrpEff)
                gl::enableWireframe();
            gl::drawSolidCircle(expressionIter->getCenter(), expressionIter->getWidth() / 1.715f );
            gl::disableWireframe();
        }
        if(aurEff){ 
            gl::enableWireframe();
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.4f);
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.4101f);
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.7967f);
            gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.8f);
            gl::disableWireframe();
        }
        if(rectEff){
            gl::pushMatrices();
            gl::translate(expressionIter->getCenter());
            gl::rotate( getElapsedSeconds() * 50 );
            gl::drawSolidRect(Rectf(Area(-expressionIter->getSize()/2, expressionIter->getSize()/2)));
            gl::popMatrices();
        }
    }
}

CINDER_APP_NATIVE( ICPApp2, RendererGl )
