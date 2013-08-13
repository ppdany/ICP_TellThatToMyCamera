#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class EffectsTestApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    CameraPersp		mCam;
	Quatf			mSpinTheWholeWorld;
	Vec3f			mVecA, mVecB;
	float			mSlerpAmt;
};

void EffectsTestApp::setup()
{
    // setup a camera that looks back at the origin from (3,3,3);
//	mCam = CameraPersp( getWindowWidth(), getWindowHeight(), 45 );
//	mCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1f, 100.0f );
//	mCam.lookAt( Vec3f( 3, 3, 3 ), Vec3f::zero() );
}

void EffectsTestApp::mouseDown( MouseEvent event )
{
}

void EffectsTestApp::update()
{
}

void EffectsTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending();
	
//	gl::setMatrices( mCam );
    gl::setMatricesWindow( getWindowSize() );
//	gl::rotate( mSpinTheWholeWorld );
    
	// draw the globe
	gl::enableWireframe();
	gl::color( ColorA( 1, 1, 0, 0.25f ) );
	gl::drawSphere( Vec3f::zero(), 1, 20 );
	gl::disableWireframe();
	
//	// draw the path
//	gl::color( Color::white() );
//	drawPathBetweenVectors( mVecA, mVecB );
//	
//	const float smallSphereRadius = 0.05f;
//	const int smallSphereSegments = 20;
//	// draw vector A
//	gl::color( ColorA( 1, 0, 0, 1 ) );
//	gl::drawSphere( mVecA, smallSphereRadius, smallSphereSegments );
//    
//	// draw vector B
//	gl::color( ColorA( 0, 0, 1, 1 ) );
//	gl::drawSphere( mVecB, smallSphereRadius, smallSphereSegments );
//    
//	// draw slerped vector
//	gl::color( ColorA( 0, 1, 1, 1 ) );
//	gl::drawSphere( mVecA.slerp( mSlerpAmt, mVecB ), smallSphereRadius, smallSphereSegments );
}

CINDER_APP_NATIVE( EffectsTestApp, RendererGl )
