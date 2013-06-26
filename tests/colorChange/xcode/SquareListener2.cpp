#include "SquareListener2.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"

using namespace ci;
using namespace ci::app;

SquareListener2::SquareListener2( app::WindowRef window )
: mWindow( window )
{
/*	mRect = Rectf( Vec2f( -40, -40 ), Vec2f( 40, 40 ) );
	mRect.offset( window->getCenter() );
	mSelected = false;
*/	
	mCbMouseDown = mWindow->getSignalMouseDown().connect( std::bind( &SquareListener2::mouseDown, this, std::_1 ) );
	mCbMouseDrag = mWindow->getSignalMouseDrag().connect( std::bind( &SquareListener2::mouseDrag, this, std::_1 ) );
}

void SquareListener2::setup( Vec2f pos, int backColour){
    mRect = Rectf( Vec2f( -40, -40 ), Vec2f( 40, 40 ) );
	mRect.offsetCenterTo(pos);
	mSelected = false;
    theColor = backColour;
}
void SquareListener2::mouseDown( MouseEvent &event )
{
	mSelected = mRect.contains( event.getPos() );
    colorEffect();
    
	// if we got selected then we handled the event, otherwise let it pass through
	event.setHandled( mSelected );
}

void SquareListener2::mouseDrag( MouseEvent &event )
{
	if( mSelected )
		mRect.offsetCenterTo( event.getPos() );
    
	// if we are selected then we handled the event, otherwise let it pass through
	event.setHandled( mSelected );
}

void SquareListener2::colorEffect(){ // 1 is red, 2 is green, 3 is blue.
    if(theColor==1){
        gl::clear( Color( 1, 0, 0 ) );
    }else if (theColor == 2){
        gl::clear( Color( 0, 1, 0 ) );
    }else if (theColor ==3){
        gl::clear( Color( 0, 0, 1 ) );
    }else{
        gl::clear( Color( 0, 0, 0 ) );
    }
}

void SquareListener2::draw(uint32_t colorHexValue)
{
    gl::color( ( mSelected ) ? Color::white() : Color::hex(colorHexValue));
	gl::drawSolidRect( mRect );
}
