#pragma once

#include "cinder/App/App.h"
#include "cinder/Vector.h"

class SquareListener {
public:
	SquareListener( ci::app::WindowRef window );
    
	void	mouseDown( ci::app::MouseEvent &event );
	void	mouseDrag( ci::app::MouseEvent &event );
	
	void	draw();
	
private:
	ci::Rectf		mRect;
   	ci::Rectf		mRect2;
   	ci::Rectf		mRect3;
	bool			mSelected;
    bool			mSelected2;
    bool			mSelected3;
	
	ci::app::WindowRef				mWindow;
	ci::signals::scoped_connection	mCbMouseDown, mCbMouseDrag;
};