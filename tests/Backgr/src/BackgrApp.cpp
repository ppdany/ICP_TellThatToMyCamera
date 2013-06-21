#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BackgrApp : public AppNative {
public:
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();
    bool redtoblack = true;
    bool blacktogreen = false;
    bool greentoblack = false;
    bool blacktoblue = false;
    bool bluetoblack = false;
    bool blacktored = false;
    
};

void BackgrApp::setup()
{
    
}

void BackgrApp::mouseDown( MouseEvent event )
{
}

void BackgrApp::update()
{
}

void BackgrApp::draw()
{
	// clear out the window with black
    //	gl::clear( Color( 0, 0, 0 ) );
    
    float gray = sin(getElapsedSeconds())*.5f+.5f;
    //cout<<gray;
    
    //from red to black
    if(redtoblack){
        gl::clear(Color(gray,0,0), true);
        if (gray<=0.01f){
            redtoblack = false;
            blacktogreen = true;
            greentoblack = false;
            blacktoblue = false;
            bluetoblack = false;
            blacktored = false;
        }
    }
    
    //from black to green
    if(blacktogreen){
        gl::clear(Color(0,gray,0), true);
        if (gray>=0.99f){
            redtoblack = false;
            blacktogreen = false;
            greentoblack = true;
            blacktoblue = false;
            bluetoblack = false;
            blacktored = false;
        }
    }
    // from green to black
    if(greentoblack){
        gl::clear(Color(0,gray,0), true);
        if (gray<=0.001f){
            redtoblack = false;
            blacktogreen = false;
            greentoblack = false;
            blacktoblue = true;
            bluetoblack = false;
            blacktored = false;
        }
    }
    
    //from black to blue
    if(blacktoblue){
        gl::clear(Color(0,0,gray), true);
        if (gray>=0.99f){
            redtoblack = false;
            blacktogreen = false;
            greentoblack = false;
            blacktoblue = false;
            bluetoblack = true;
            blacktored = false;
        }
    }
    // from blue to black
    if(bluetoblack){
        gl::clear(Color(0,0,gray), true);
        if (gray<=0.001f){
            redtoblack = false;
            blacktogreen = false;
            greentoblack = false;
            blacktoblue = false;
            bluetoblack = false;
            blacktored = true;
        }
    }
    
    //from black to red and start over
    if(blacktored){
        gl::clear(Color(gray,0,0), true);
        if (gray>=0.9f){
            redtoblack = true;
            blacktogreen = false;
            greentoblack = false;
            blacktoblue = false;
            bluetoblack = false;
            blacktored = false;
        }
    }
}

CINDER_APP_NATIVE( BackgrApp, RendererGl )
