#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "SquareListener2.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
#include "cinder/Text.h"
//#include "cinder/

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
    void resize(Res);
    
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

void CamColorApp::keyDown(KeyEvent event){
    // KEYBOARD commands for camera
    if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
	else if( event.getChar() == ' ' ) {
		mCaptures.back()->isCapturing() ? mCaptures.back()->stop() : mCaptures.back()->start();
	}
	else if( event.getChar() == 'r' ) {
		// retain a random surface to exercise the surface caching code
		int device = rand() % ( mCaptures.size() );
		mRetainedSurfaces.push_back( mCaptures[device]->getSurface() );
		console() << mRetainedSurfaces.size() << " surfaces retained." << std::endl;
	}
	else if( event.getChar() == 'u' ) {
		// unretain retained surface to exercise the Capture's surface caching code
		if( ! mRetainedSurfaces.empty() )
			mRetainedSurfaces.pop_back();
		console() << mRetainedSurfaces.size() << " surfaces retained." << std::endl;
	}
}

void CamColorApp::mouseDown( MouseEvent event )
{
}

void CamColorApp::update()
{
    for( vector<CaptureRef>::iterator cIt = mCaptures.begin(); cIt != mCaptures.end(); ++cIt ) {
		if( (*cIt)->checkNewFrame() ) {
			Surface8u surf = (*cIt)->getSurface();
			mTextures[cIt - mCaptures.begin()] = gl::Texture::create( surf );
		}
	}
}

void CamColorApp::draw()
{
	//BACKGROUND
    // clear out the window with black
    //	gl::clear( Color( 0, 0, 0 ) );
    mSquare->draw(0xF73C02);
    mSquare2->draw(0x71C462);
    mSquare3->draw(0x0589F5);
    
    //CAMERA
    gl::enableAlphaBlending();
	//gl::clear( Color::black() );
	if( mCaptures.empty() )
		return;
    
	float width = getWindowWidth() / mCaptures.size();
	float height = width / ( WIDTH / (float)HEIGHT );
	float x = 0, y = ( getWindowHeight() - height ) / 2.0f;
	for( vector<CaptureRef>::iterator cIt = mCaptures.begin(); cIt != mCaptures.end(); ++cIt ) {
		// draw the latest frame
		gl::color( Color::white() );
		if( mTextures[cIt-mCaptures.begin()] )
			gl::draw( mTextures[cIt-mCaptures.begin()], Rectf( x, y, x + width, y + height ) );
        
		// draw the name
		gl::color( Color::black() );
		gl::draw( mNameTextures[cIt-mCaptures.begin()], Vec2f( x + 10 + 1, y + 10 + 1 ) );
		gl::color( Color( 0.5, 0.75, 1 ) );
		gl::draw( mNameTextures[cIt-mCaptures.begin()], Vec2f( x + 10, y + 10 ) );
        
		x += width;
	}
}

CINDER_APP_NATIVE( CamColorApp, RendererGl )
