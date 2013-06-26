#pragma once

#include "cinder/App/App.h"
#include "cinder/Vector.h"

class SquareListener2 {
public:
	SquareListener2( ci::app::WindowRef window );
    
    void    setup(cinder::Vec2f pos, int backColour);
	void	mouseDown( ci::app::MouseEvent &event );
	void	mouseDrag( ci::app::MouseEvent &event );
    void    colorEffect();
	
	void	draw(uint32_t colorHexValue);
    int     theColor;
	
private:
	ci::Rectf		mRect;
	bool			mSelected;
    //int            theColor;
	
	ci::app::WindowRef				mWindow;
	ci::signals::scoped_connection	mCbMouseDown, mCbMouseDrag;
};