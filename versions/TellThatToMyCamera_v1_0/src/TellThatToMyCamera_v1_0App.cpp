/*
 * Tell That To My Camera v1.0
 * Copyright (c) 2013. Jose Daniel Leal Avila and The University of Edinburgh. All rights reserved.
 * The author and the University of Edinburgh retain the right to reproduce and publish this software for non-commercial purposes. Permission to use, copy, modify, and distribute this software and its documentation for educational, research, and not-for-profit purposes, without fee and without a signed licensing agreement, is hereby granted, provided that the above copyright notice, this paragraph and the following disclaimer appear in all copies, modifications, and distributions. The rest of the present paragraphs can optionally be included. Applications to make other use of the material should be addressed in the first instance to the author at jdanielleal[AT]gmail[DOT]com, in the second instance, to the MSc by Res. in Interdisciplinary Creative Practices programme administrator and in the final instance, to Copyright Permissions, Division of Informatics, The University of Edinburgh, 80 South Bridge, Edinburgh EH1 1HN, Scotland. The authors and the University of Edinburgh retain the right to reproduce and publish this software for non-commercial purposes.
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
#include "CinderOpenCv.h"
#include <fstream>
#include <iostream>

using namespace ci;
using namespace ci::app;
using namespace std;

class TellThatToMyCamera_v1_0App : public AppNative {
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
};

void TellThatToMyCamera_v1_0App::read_csv( string filename, vector<cv::Mat>& images, vector<int>& labels, char separator = ';'){
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

void TellThatToMyCamera_v1_0App::setup()
{
    mExpressionsCascade.load(getAssetPath("haarcascade_frontalface_alt.xml").string());
    mPath= getAssetPath("ppdtest.csv").string();
    
	mCapture = Capture( 640, 480 );                 // Camera settings
	mCapture.start();
    
    read_csv(mPath, mDBimgFaces, mDBLabels);        // Read DB of faces for FaceRec algorithm
    mFisherFaceRec->train(mDBimgFaces, mDBLabels);  // Train the Fisher Face Recognizer algorithm
}

void TellThatToMyCamera_v1_0App::updateExpressions (Surface cameraImage){
	cv::Mat grayCameraImage( toOcv( cameraImage, CV_8UC1 ) );   // create a grayscale copy of the input image
   	cv::equalizeHist( grayCameraImage, grayCameraImage );       // equalize the histogram for (just a little) more accuracy
    mExpressions.clear();                                       // clear out the previously deteced expressions
    mPredictions.clear();
    vector<cv::Rect> expressions;
    
    // Next is to detect the faces and iterate them, appending them to mExpressions
    mExpressionsCascade.detectMultiScale(grayCameraImage, expressions);
    
    // At this point the position of the faces has been calculated!
    // Now it's time to get the faces, make a prediction and save it for the video.
    
    cv::Mat graySq(100,100,CV_8UC1);    // gray square for assigning the proper size of the resized detected faces
    
    for(vector<cv::Rect>::const_iterator expressionIter = expressions.begin(); expressionIter != expressions.end(); ++expressionIter){
        // Get the process face by face (in case there's more than one face in the video frame image)
        Rectf expressionRect(fromOcv(*expressionIter));
        mExpressions.push_back(expressionRect);
        
        cv::Rect face_i (*expressionIter);                      // Rect with data (size and position) of the detected face
        cv::Mat face = grayCameraImage(face_i);                 // Image containing the detected face
        cv::Mat face_resized;                                   // Image for the resized version of the detected face
        cv::resize(face, face_resized, graySq.size(), 1, 1, cv::INTER_CUBIC); // resizes the image
        // cv::resize(face, face_resized, graySq.size(), 0, 0, cv::INTER_LINEAR);
        
        // Now, perform the EXPRESSION PREDICTION!!!
        int predicted = mFisherFaceRec->predict(face_resized);
        mPredictions.push_back(predicted);                      // put the corresponding label to the corresponding face
    }
}

void TellThatToMyCamera_v1_0App::update()
{
	if( mCapture.checkNewFrame() ) {
		Surface surface = mCapture.getSurface();
		mCameraTexture = gl::Texture(surface);
        updateExpressions(surface);
	}
}

void TellThatToMyCamera_v1_0App::ColourTheAura(int label){
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
    }*/
    
    /* The SWITCH version */
     switch ( label ) // Uses TEST DB
     {
     case 0:
     gl::color( ColorA( 1.f, 1.f, 1.f, 0.45f ) );
     break;
     case 1:
     gl::color( ColorA( 1.f, 0.f, 0.f, 0.45f ) );
     break;
     case 2:
     gl::color( ColorA( (50.f*(1.f/255.f)), (205.f*(1.f/255.f)), (50.f*(1.f/255.f)), 0.45f ) );
     break;
     case 3:
     gl::color( ColorA( (139.f*(1.f/255.f)), (69.f*(1.f/255.f)), (19.f*(1.f/255.f)), 0.45f ) );
     break;
     case 4:
     gl::color( ColorA( 0.f, 0.f, 1.f, 0.45f ) );
     break;
     case 5:
     gl::color( ColorA( 1.f, (215.f*(1.f/255.f)), 0, 0.45f ) );
     break;
     case 6:
     gl::color( ColorA( 0.f, 0.f, 0.f, 0.45f ) );
     break;
     case 7:
     gl::color( ColorA( 1.f, (165.f*(1.f/255.f)), 0, 0.45f ) );
     break;
     case 8:
     gl::color( ColorA( (128.f*(1.f/255.f)), 0.f, (128.f*(1.f/255.f)), 0.45f ) );
     break;
     case 9:
     gl::color( ColorA( (238.f*(1.f/255.f)), (15.f*(1.f/255.f)), (151.f*(1.f/255.f)), 0.45f ) );
     break;
     default:
     gl::color( ColorA( 1.f, 1.f, 1.f, 0.1f ) );
     break;
     }
}

void TellThatToMyCamera_v1_0App::draw()
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
    
    // Draw the corresponding coloured squares over the identified expressions
    for( vector<Rectf>::const_iterator expressionIter = mExpressions.begin(); expressionIter != mExpressions.end(); ++expressionIter ){
        ColourTheAura(mPredictions[expressionIter-mExpressions.begin()]);
        gl::drawSolidRect( *expressionIter );
    }
    
}

CINDER_APP_NATIVE( TellThatToMyCamera_v1_0App, RendererGl )
