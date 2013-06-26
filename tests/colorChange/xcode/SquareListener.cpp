#include "SquareListener.h"
#include "cinder/gl/gl.h"
#include "cinder/Rect.h"

using namespace ci;
using namespace ci::app;

SquareListener::SquareListener( app::WindowRef window )
: mWindow( window )
{
    mRect = Rectf( Vec2f( -40, -40 ), Vec2f( 40, 40 ) );
    mRect.offsetCenterTo(Vec2f(50,50));
    mRect2 = Rectf( Vec2f( -40, -40 ), Vec2f( 40, 40 ) );
    mRect2.offsetCenterTo(Vec2f(50,150));
    mRect3 = Rectf( Vec2f( -40, -40 ), Vec2f( 40, 40 ) );
    mRect3.offsetCenterTo(Vec2f(50,250));
	//mRect.offset( window->getCenter() );
    
	mSelected = false;
    mSelected2 = false;
    mSelected3 = false;
	
	mCbMouseDown = mWindow->getSignalMouseDown().connect( std::bind( &SquareListener::mouseDown, this, std::_1 ) );
	mCbMouseDrag = mWindow->getSignalMouseDrag().connect( std::bind( &SquareListener::mouseDrag, this, std::_1 ) );
}

void SquareListener::mouseDown( MouseEvent &event )
{
	mSelected = mRect.contains( event.getPos() );
    mSelected2 = mRect2.contains( event.getPos() );
    mSelected3 = mRect3.contains( event.getPos() );
    std::cout<<mSelected<<std::endl;
    std::cout<<mSelected2<<std::endl;
    std::cout<<+mSelected3<<std::endl;
    
    std::cout<<std::endl;
    
    
    /*if (mSelected){
        event.setHandled(mSelected);
        std::cout<<mSelected<<std::endl;
        std::cout<<std::endl;
    }else if (mSelected2){
        event.setHandled(mSelected2);
        std::cout<<+mSelected2<<std::endl;
        std::cout<<std::endl;
    }else if (mSelected3){
        event.setHandled(mSelected3);
        std::cout<<+mSelected3<<std::endl;
        std::cout<<std::endl;
    }*/
    
    /*
	// if we got selected then we handled the event, otherwise let it pass through
	event.setHandled( mSelected );
	event.setHandled( mSelected2 );
	event.setHandled( mSelected3 );
    */
    
}

void SquareListener::mouseDrag( MouseEvent &event )
{
	if( mSelected ){
        mRect.offsetCenterTo( event.getPos() );
        //event.setHandled( mSelected );
    }
    if( mSelected2 ){
        mRect2.offsetCenterTo( event.getPos() );
        //event.setHandled( mSelected2 );
    }
    if( mSelected3 ){
        mRect3.offsetCenterTo( event.getPos() );
        //event.setHandled( mSelected3 );
    }
    
    /*
    if( mSelected )
        mRect.offsetCenterTo( event.getPos() );
    if( mSelected2 )
		mRect2.offsetCenterTo( event.getPos() );
    if( mSelected3 )
		mRect3.offsetCenterTo( event.getPos() );
    
	// if we are selected then we handled the event, otherwise let it pass through
	event.setHandled( mSelected );
    event.setHandled(mSelected2);
    event.setHandled(mSelected3);
    */
}

void SquareListener::draw()
{
    if(mSelected){
        gl::color( ( mSelected ) ? Color::white() : Color::black());
    }
    if(mSelected2){
        gl::color( ( mSelected2 ) ? Color::white() : Color::hex(0xF5F505));
    }
    if(mSelected3){
        gl::color( ( mSelected3 ) ? Color::white() : Color::hex(0xC81ADB));
    }
	/*
    gl::color( ( mSelected ) ? Color::white() : Color::black());
    gl::color( ( mSelected2 ) ? Color::white() : Color::black());
    gl::color( ( mSelected3 ) ? Color::white() : Color::hex(0xC81ADB));
	*/
    gl::drawSolidRect( mRect );
    gl::drawSolidRect( mRect2 );
    gl::drawSolidRect( mRect3 );
    
    
    
}
