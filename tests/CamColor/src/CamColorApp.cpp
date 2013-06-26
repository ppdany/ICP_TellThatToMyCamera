#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "SquareListener2.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
#include "cinder/Text.h"

using namespace ci;
using namespace ci::app;
using namespace std;

static const int WIDTH = 640, HEIGHT = 480;

class CamColorApp : public AppNative {
public:
	void setup();
    void keyDown(KeyEvent event);
	void mouseDown( MouseEvent event );
	void update();
	void draw();
    
    SquareListener2 *mSquare;
    SquareListener2 *mSquare2;
    SquareListener2 *mSquare3;

private:
    Vec2f pos1=Vec2f(50,50);
    Vec2f pos2=Vec2f(50,150);
    Vec2f pos3=Vec2f(50,250);
    vector<CaptureRef>		mCaptures;
	vector<gl::TextureRef>	mTextures;
	vector<gl::TextureRef>	mNameTextures;
	vector<Surface>			mRetainedSurfaces;
    
    
};

void CamColorApp::setup()
{
 	mSquare = new SquareListener2( getWindow() );
    mSquare2 = new SquareListener2( getWindow() );
    mSquare3 = new SquareListener2( getWindow() );
    
    mSquare->setup(pos1, 1);
    mSquare2->setup(pos2, 2);
    mSquare3->setup(pos3, 3);
    
    gl::clear( Color( 0, 0, 0 ) );
    
    //FOR Camera
    // list out the devices
	vector<Capture::DeviceRef> devices( Capture::getDevices() );
	for( vector<Capture::DeviceRef>::const_iterator deviceIt = devices.begin(); deviceIt != devices.end(); ++deviceIt ) {
		Capture::DeviceRef device = *deviceIt;
		console() << "Found Device " << device->getName() << " ID: " << device->getUniqueId() << std::endl;
		try {
			if( device->checkAvailable() ) {
				mCaptures.push_back( Capture::create( WIDTH, HEIGHT, device ) );
				mCaptures.back()->start();
                
				// placeholder text
				mTextures.push_back( gl::TextureRef() );
                
				// render the name as a texture
				TextLayout layout;
				layout.setFont( Font( "Arial", 24 ) );
				layout.setColor( Color( 1, 1, 1 ) );
				layout.addLine( device->getName() );
				mNameTextures.push_back( gl::Texture::create( layout.render( true ) ) );
			}
			else
				console() << "device is NOT available" << std::endl;
		}
		catch( CaptureExc & ) {
			console() << "Unable to initialize device: " << device->getName() << endl;
		}
	}
}

void CamColorApp::mouseDown( MouseEvent event )
{
}

void CamColorApp::update()
{
}

void CamColorApp::draw()
{
	// clear out the window with black
    //	gl::clear( Color( 0, 0, 0 ) );
    
    mSquare->draw(0xF73C02);
    mSquare2->draw(0x71C462);
    mSquare3->draw(0x0589F5);
}

CINDER_APP_NATIVE( CamColorApp, RendererGl )
