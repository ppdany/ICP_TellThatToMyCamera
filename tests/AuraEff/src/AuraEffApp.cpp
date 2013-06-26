#include "cinder/app/AppBasic.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/Texture.h"
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "ParticleController.h"

// RESOLUTION refers to the number of pixels
// between neighboring particles. If you increase
// RESOULTION to 10, there will be 1/4th as many particles.
// Setting RESOLUTION to 1 will create 1 particle for
// every pixel in the app window.
#define RESOLUTION 5

using namespace ci;
using namespace ci::app;

class AuraEffApp : public AppBasic {
public:
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event );
    void mouseDown( MouseEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void setup();
	void update();
	void draw();
	
	Channel32f mChannel;
	gl::Texture	mTexture;
	
	Vec2i mMouseLoc;
	
	ParticleController mParticleController;
	
	bool mDrawParticles;
	bool mDrawImage;
};

void AuraEffApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
}

void AuraEffApp::mouseMove( MouseEvent event )
{
	mMouseLoc = event.getPos();
}

void AuraEffApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void AuraEffApp::mouseDown( MouseEvent event ) {
    if( event.isRight() && event.isShiftDown() ) {
        console() << "Secret thing happened!" << std::endl;
    }
}

void AuraEffApp::keyDown( KeyEvent event )
{
	if( event.getChar() == '1' ){
		mDrawImage = ! mDrawImage;
	} else if( event.getChar() == '2' ){
		mDrawParticles = ! mDrawParticles;
	}
}

void AuraEffApp::setup()
{
	Url url( "http://libcinder.org/media/tutorial/paris.jpg" );
	mChannel = Channel32f( loadImage( loadUrl( url ) ) );
	mTexture = mChannel;
    
	mParticleController = ParticleController( RESOLUTION );
	
	mMouseLoc = Vec2i( 0, 0 );
	
	mDrawParticles = true;
	mDrawImage = false;
}

void AuraEffApp::update()
{
	if( ! mChannel ) return;
	
	mParticleController.update( mChannel, mMouseLoc );
}

void AuraEffApp::draw()
{
	gl::clear( Color( 0, 0, 0 ), true );
	
	if( mDrawImage ){
		mTexture.enableAndBind();
		gl::draw( mTexture, getWindowBounds() );
	}
	
	if( mDrawParticles ){
		glDisable( GL_TEXTURE_2D );
		mParticleController.draw();
	}
}

CINDER_APP_BASIC( AuraEffApp, RendererGl )
