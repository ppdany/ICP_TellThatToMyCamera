#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Rand.h"
#include "ParticleController.h"

#define RESOLUTION 5

using namespace ci;
using namespace ci::app;
using namespace std;

class surroundAuraApp : public AppNative {
public:
   	void prepareSettings( Settings *settings );
	void mouseDown( MouseEvent event );
    void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
    void setup();
	void update();
	void draw();
    gl::Texture myImg;
    
    Channel32f mChannel;
	gl::Texture	mTexture;
	
	Vec2i mMouseLoc;
	
	ParticleController mParticleController;
	
	bool mDrawParticles;
	//bool mDrawImage;
    
    bool redtoblack = true;
    bool blacktogreen = false;
    bool greentoblack = false;
    bool blacktoblue = false;
    bool bluetoblack = false;
    bool blacktored = false;
    
};

void surroundAuraApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
}

void surroundAuraApp::mouseMove( MouseEvent event )
{
	mMouseLoc = event.getPos();
}

void surroundAuraApp::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void surroundAuraApp::mouseDown( MouseEvent event ) {
    if( event.isRight() && event.isShiftDown() ) {
        console() << "Special thing happened!" << std::endl;
    }
}

void surroundAuraApp::setup()
{
    //myImg = gl::Texture(loadImage(loadResource("img.jpg")));
    //mChannel = Channel32f( loadImage( loadUrl( url ) ) );
	mTexture = mChannel;
    
	mParticleController = ParticleController( RESOLUTION );
	
	mMouseLoc = Vec2i( 0, 0 );
	
	mDrawParticles = true;
	//mDrawImage = false;
  
}


void surroundAuraApp::update()
{
    if( ! mChannel ) return;
    
	mParticleController.update( mChannel, mMouseLoc );
}

void surroundAuraApp::draw()
{

    /*// IMAGE
    if(myImg)
        gl::draw(myImg, getWindowBounds());
     
     if( mDrawImage ){
     mTexture.enableAndBind();
     gl::draw( mTexture, getWindowBounds() );
     }
    */
    
    //PARTICLES
    if( mDrawParticles ){
		glDisable( GL_TEXTURE_2D );
		mParticleController.draw();
	}
     
     //COLOR EFFECTS
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

CINDER_APP_NATIVE( surroundAuraApp, RendererGl )
