#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
using namespace ci;
using namespace ci::app;

using namespace std;

class CinderProjectApp : public AppNative {
public:
	void setup();
	void update();
	void draw();
    void mouseMove(MouseEvent event);
    //    void prepareSettings( Settings *settings );
    Vec2i m_mouseLoc;
private:
    void ChangeBackground();
    Color m_backbColor;
    
    
};


/*void CinderProjectApp::prepareSettings( Settings *settings ){
 settings->setWindowSize( 200, 600 );
 settings->setFrameRate( 5.0f );
 }*/

void CinderProjectApp::ChangeBackground(){
    m_backbColor = Color(Rand::randInt(255),Rand::randInt(255),Rand::randInt(255));
}

void CinderProjectApp::setup()
{
    //    Settings->setWindowSize(200, 500);
    gl::clear( Color(0,0,0) );
    
}

void CinderProjectApp::mouseMove(MouseEvent event){
    m_mouseLoc = event.getPos();
}


void CinderProjectApp::update()
{
    ChangeBackground();
}

void CinderProjectApp::draw()
{
	// clear out the window with black
    //	gl::clear( Color(0,0,0) );
    Vec2f v = Vec2f((float) m_mouseLoc.x,(float)m_mouseLoc.y);
    //    Vec2f v = Vec2f(100.0f,100.0f);
    gl::drawSolidCircle(v, 10.0f);
}

CINDER_APP_NATIVE( CinderProjectApp, RendererGl )
