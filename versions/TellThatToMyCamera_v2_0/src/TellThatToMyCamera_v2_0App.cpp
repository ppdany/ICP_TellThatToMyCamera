/*
 * Tell That To My Camera v2.0
 * Copyright (c) 2013. Jose Daniel Leal Avila and The University of Edinburgh. All rights reserved.
 * The author and the University of Edinburgh retain the right to reproduce and publish this software for non-commercial purposes. Permission to use, copy, modify, and distribute this software and its documentation for educational, research, and not-for-profit purposes, without fee and without a signed licensing agreement, is hereby granted, provided that the above copyright notice, this paragraph and the following disclaimer appear in all copies, modifications, and distributions. The rest of the present paragraphs can optionally be included. Applications to make other use of the material should be addressed in the first instance to the author at jdanielleal[AT]gmail[DOT]com, in the second instance, to the MSc by Res. in Interdisciplinary Creative Practices programme administrator and in the final instance, to Copyright Permissions, Division of Informatics, The University of Edinburgh, 80 South Bridge, Edinburgh EH1 1HN, Scotland. The author and the University of Edinburgh retain the right to reproduce and publish this software for non-commercial purposes.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 * This software has been developed as part of the final project
 * for the programme MSc. by Res. in Interdisciplinary Creative Practices.
 * Author: Jose Daniel Leal Avila
 * jdanielleal@gmail.com        j.leal-avila@sms.ed.ac.uk
 * University student id. s1217971
 * Programme Director: Simon Biggs  s.biggs@ed.ac.uk
 *
 * MSc by Research in Interdisciplinary Creative Practices
 * The University of Edinburgh. 
 * Edinburgh College of Art. 
 * Edinburgh School of Architecture and Landscape Architecture.
 *
 */
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

class TellThatToMyCamera_v2_0App : public AppNative {
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
};

void TellThatToMyCamera_v2_0App::read_csv( string filename, vector<cv::Mat>& images, vector<int>& labels, char separator = ';'){
    ifstream inFile(filename.c_str());
    string line, path, classlabel;
    while (getline(inFile, line)) {                 // While the file has lines
        stringstream liness(line);                  // separate the line
        getline(liness, path, separator);           // put the first part of the line in path
        getline(liness, classlabel);                // put the second part of the line in classlabel
        if(!path.empty() && !classlabel.empty()) {  // if there are still paths and labels
            ci::Surface8u surface(loadImage(path));       // assign the image to a surface
            cv::Mat graySurface(toOcv(surface, CV_8UC1)); // convert surface to a grayscale Mat variable (OpenCV)
            images.push_back(graySurface);                // put the grayscale image in the img vector
            labels.push_back(atoi(classlabel.c_str()));   // assign the label
        }
    }
}

void TellThatToMyCamera_v2_0App::setup()
{
    // Load necessary files for the machine learning process
    mExpressionsCascade.load(getAssetPath("haarcascade_frontalface_alt.xml").string());
    mPath= getAssetPath("emotions.csv").string();
    // FOR TEST DB
    // mPath= getAssetPath("ppdtest.csv").string();
    // Setup flags for effects. The semi-transparent circles effect is true to set it as the default effect
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
}

void TellThatToMyCamera_v2_0App::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )                    // Enter OR leave full-screen mode with F key
		setFullScreen( ! isFullScreen() );
    if( event.getCode() == KeyEvent::KEY_ESCAPE ){  // Leave full-screen mode with ESC key
        if (isFullScreen())
            setFullScreen(!isFullScreen());
    }
    if( event.getChar() == 'a' )                    // activate OR deactivate aurEff with A key
		aurEff = !aurEff;
    if( event.getChar() == 'c' )                    // activate OR deactivate circEff with C key
		circEff = !circEff;
    if( event.getChar() == 'r' )                    // activate OR deactivate rectEff with R key
		rectEff = !rectEff;
    if( event.getChar() == 's' )                    // activate OR deactivate sprSlrpEff with S key
		sprSlrpEff = !sprSlrpEff;    
}

void TellThatToMyCamera_v2_0App::updateExpressions (Surface cameraImage){
	cv::Mat grayCameraImage( toOcv( cameraImage, CV_8UC1 ) ); // create a grayscale copy of the input image
   	cv::equalizeHist( grayCameraImage, grayCameraImage );	  // equalize the histogram for (just a little) more accuracy
    mExpressions.clear();                                     // clear out the previously deteced expressions
    mPredictions.clear();                                     // clear out the previously deteced predictions
    vector<cv::Rect> expressions;                             // Rect vector for the faces calculations with OpenCV
    
    // Detect the faces in the frame image and iterate them, appending them to 'expressions'
    mExpressionsCascade.detectMultiScale(grayCameraImage, expressions);
    
    // At this point the position of the faces has been calculated,
    // now it's time to get that information for each face, make a prediction and save it for the video effects.
    
    cv::Mat graySq(100,100,CV_8UC1);                // gray sq. for assigning the right size when resizing the detected faces
    
    for(vector<cv::Rect>::const_iterator expressionIter = expressions.begin(); expressionIter != expressions.end(); ++expressionIter){
        // Get the process face by face (in case there's more than one face in the video frame image)
        Rectf expressionRect(fromOcv(*expressionIter));
        mExpressions.push_back(expressionRect);
        
        cv::Rect face_i (*expressionIter);                  // Rect with data (size and position) of the detected face
        cv::Mat face = grayCameraImage(face_i);             // Image containing the detected face
        cv::Mat face_resized;                               // Image for the resized version of the detected face
        cv::resize(face, face_resized, graySq.size(), 1, 1, cv::INTER_CUBIC); // resizes the image for proper size
        
        // Now, perform the EXPRESSION PREDICTION
        int predicted = mFisherFaceRec->predict(face_resized);
        mPredictions.push_back(predicted);                  // put the corresponding label to the corresponding face        
    }
}

void TellThatToMyCamera_v2_0App::update()
{
	if( mCapture.checkNewFrame() ) {                        // As long as the camera sends new frames...
		Surface surface = mCapture.getSurface();            // assign the frame to a surface
		mCameraTexture = gl::Texture(surface);              // assign the surface to a texture
        updateExpressions(surface);                         // update info. about the expressions
	}
}

void TellThatToMyCamera_v2_0App::ColourTheAura(int label){
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
     none           -   Transparent     255,255,255 with 0.1 alpha   ***If for some strange reason a bug happens.***
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

void TellThatToMyCamera_v2_0App::draw()
{
    gl::clear();
	if( ! mCameraTexture )              // In case the program failed to load a new frame
		return;
    
    // get window size for proper display (full screen or default screen size)
    int w = getWindowWidth();
    int h = getWindowHeight();
    //int h = w / ( WIDTH / (float)HEIGHT ); // h=1080 in fullscreen... This would fill the screen but lose part of the upper and (specially) the lower area
    
    // determine scale
    float s = std::min<float>( ((float) w / WIDTH), ((float) h / HEIGHT) ); //s=2.25 with ratio loss, s=1.875 if no ratio loss
    
    // center content
    int ws = (int) (WIDTH * s + 0.5f);
    int hs = (int) (HEIGHT * s + 0.5f);
    int x = (w - ws) / 2;
    int y = (h - hs) / 2;
    
    // move origin and scale content
    gl::pushModelView();
    gl::setMatricesWindow( getWindowSize() );
    gl::translate(x, y, 0.0f);
    gl::scale(s, s, s);
    
    // Finally, draw everything
    gl::enableAlphaBlending();
    gl::color( Color::white() );
    gl::draw( mCameraTexture );
    mCameraTexture.disable();
    
    // Draw the corresponding effects over the identified expressions
    for( vector<Rectf>::const_iterator expressionIter = mExpressions.begin(); expressionIter != mExpressions.end(); ++expressionIter ){
        ColourTheAura(mPredictions[expressionIter-mExpressions.begin()]); // Set colour
        // Draw a circle (no fill) around the face so that there's always at least this effect
        gl::enableWireframe();
        gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.7f);
        gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.705f);
        gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.7101f);
        gl::drawStrokedCircle(expressionIter->getCenter(), expressionIter->getWidth()/ 1.714f);
        gl::disableWireframe();
        // Check what effects are activated and produce it (or them)
        if(circEff){        // Activated by default. This effect draws a semi-transparent circle (filled) over a face
            if(sprSlrpEff)  // This gives a visual effect that looks as if the transparency intensity of the circle changes from the center outwards. It is for representing "the source energy of the aura" 
                gl::enableWireframe();
            gl::drawSolidCircle(expressionIter->getCenter(), expressionIter->getWidth() / 1.715f );
            gl::disableWireframe();
        }
        if(aurEff){ // This effect draws circles (with no fill) over a face that look as moving waves, representing an aura that emanates from the user's face
            gl::enableWireframe();
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/ 1.4f);
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/ 1.4101f);
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/(float(Rand::randFloat(1.3f,1.8f))));
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/ 1.7967f);
            gl::drawStrokedCircle(expressionIter->getCenter(),expressionIter->getWidth()/ 1.8f);
            gl::disableWireframe();
        }
        if(rectEff){ // This draws a semi-transparent rotating square (with fill) over a face
            gl::pushMatrices();
            gl::translate(expressionIter->getCenter());
            gl::rotate( getElapsedSeconds() * 50 );
            gl::drawSolidRect(Rectf(Area(-expressionIter->getSize()/2, expressionIter->getSize()/2)));
            gl::popMatrices();
        }
    }
}

CINDER_APP_NATIVE( TellThatToMyCamera_v2_0App, RendererGl )
