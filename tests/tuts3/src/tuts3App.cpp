#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "ParticleController.h"
#include "cinder/Channel.h"
#include "cinder/Vector.h"

#define RESOLUTION 5
using namespace ci;
using namespace ci::app;
using namespace std;

class tuts3App : public AppNative {
public:
    void prepareSettings( Settings *settings );
	void setup();
    void keyDown( KeyEvent event);
	void mouseDown( MouseEvent event );
    void mouseMove(MouseEvent event);
    void mouseDrag(MouseEvent event);
	void update();
	void draw();
    
    Channel32f mChannel;
    gl::Texture mImg;
    
    ParticleController mParticleController;
    
    bool mRenderImage, mRenderParticles;
    Vec2i mMouseLoc;
};

void tuts3App::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
}

void tuts3App::setup()
{
    //Url url( "http://upload.wikimedia.org/wikipedia/commons/5/51/Small_Red_Rose.JPG" );
    //Url url ("http://www.dicts.info/img/ud/rose2.jpg");
    Url url( "http://libcinder.org/media/tutorial/paris.jpg" );
    //mImg = gl::Texture( loadImage( loadUrl( url ) ) );
    mChannel = Channel32f(loadImage(loadUrl(url)));
    mImg = mChannel;
    
    mParticleController = ParticleController(RESOLUTION);
    
    mMouseLoc = Vec2i(0,0);
    
    mRenderParticles = true;
    mRenderImage = false;
    
}

void tuts3App::keyDown(KeyEvent event){
    if(event.getChar() == '1' ){
        mRenderImage = !mRenderImage;
    }else if (event.getChar()== '2'){
        mRenderParticles = !mRenderParticles;
    }
}

void tuts3App::mouseDown( MouseEvent event )
{
    if( event.isRight() && event.isShiftDown() ) {
        console() << "Hidden thing happened!" << std::endl;
    }
}

void tuts3App::mouseMove(MouseEvent event){
    mMouseLoc = event.getPos();
}
void tuts3App::mouseDrag(MouseEvent event){
    mouseMove(event);
}

void tuts3App::update()
{
    if (!mChannel) return;
    mParticleController.update(mChannel, mMouseLoc);
}

void tuts3App::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ), true );
    
    /*if(mImg)
     gl::draw(mImg, getWindowBounds());
     float x = cos( getElapsedSeconds() )*100.0f ;
     float y = sin( getElapsedSeconds() )*100.0f ;
     gl::drawSolidCircle( Vec2f( x, y )+getWindowSize()/2, abs(x) );
     */
    if (mRenderImage){
        mImg.enableAndBind();
        gl::draw(mImg, getWindowBounds());
    }
    
    if(mRenderParticles){
        glDisable(GL_TEXTURE_2D);
        mParticleController.draw();
    }
    
}

CINDER_APP_NATIVE( tuts3App, RendererGl )
