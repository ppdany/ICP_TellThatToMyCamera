#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "SquareListener2.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class colorChangeApp : public AppNative {
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
    
    SquareListener2 *mSquare;
    SquareListener2 *mSquare2;
    SquareListener2 *mSquare3;
    
    Vec2f pos1=Vec2f(50,50);
    Vec2f pos2=Vec2f(50,150);
    Vec2f pos3=Vec2f(50,250);
    
    
};

void colorChangeApp::setup()
{
 	mSquare = new SquareListener2( getWindow() );
    mSquare2 = new SquareListener2( getWindow() );
    mSquare3 = new SquareListener2( getWindow() );
    
    mSquare->setup(pos1, 1);
    mSquare2->setup(pos2, 2);
    mSquare3->setup(pos3, 3);
    
    gl::clear( Color( 0, 0, 0 ) );
}

void colorChangeApp::mouseDown( MouseEvent event )
{
}

void colorChangeApp::update()
{
}

void colorChangeApp::draw()
{
	// clear out the window with black
    //	gl::clear( Color( 0, 0, 0 ) );
    
    /*
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
    */
    
    mSquare->draw(0xF73C02);
    mSquare2->draw(0x71C462);
    mSquare3->draw(0x0589F5);
}

CINDER_APP_NATIVE( colorChangeApp, RendererGl )
