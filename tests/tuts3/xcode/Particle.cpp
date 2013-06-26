#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Particle::Particle()
{
}

Particle::Particle( Vec2f loc )
{
	mLoc	= loc;
	mDir	= Rand::randVec2f();
    mDirToCursor = Vec2f::zero();
	//mVel	= Rand::randFloat( 5.0f );
    mVel = 0.0f;
    mRadius = 0.0f;
    mScale = 3.0f;
    //mRadius = Rand::randFloat(1.0, 5.0f);
	//mRadius = cos(mLoc.y*0.1f) + sin(mLoc.x*0.1f) + 2.0f;
    //mRadius = ( sin( mLoc.y * mLoc.x) + 1.0f) *2.0f;
    //float xyOffset = sin( cos( sin( mLoc.y * 0.3183f ) + cos( mLoc.x * 0.3183f ) ) ) + 1.0f;
    //mRadius = xyOffset * xyOffset * 1.8f;
    //mRadius	= 4.0f;
}

void Particle::update( const Channel32f &channel, const Vec2i &mouseLoc)
{
    mDirToCursor = mouseLoc - mLoc;
    
	/*float time = app::getElapsedSeconds();
     float gray = sin( time ) * 0.5f + 0.5f;
     mLoc += mDir * gray; */
    //mLoc += mDir * mVel;
    
    float gray = channel.getValue(mLoc);
    mColor = Color( gray, gray, gray);
    
    mDirToCursor.safeNormalize();
    mRadius = channel.getValue(mLoc)*mScale;
    
    //Rectf rect(mLoc.x, mLoc.y, mLoc.x+mRadius, mLoc.y+mRadius);
    //gl::drawSolidRect(rect);
    Vec2f newLoc = mLoc+mDirToCursor *100.0f;
    newLoc.x = constrain(newLoc.x, 0.0f, channel.getWidth() - 1.0f);
    newLoc.y = constrain(newLoc.y, 0.0f, channel.getHeight() - 1.0f);
    
    mRadius = channel.getValue(newLoc)*mScale;
    
}

void Particle::draw()
{
    //gl::color(mColor);
	//gl::drawSolidCircle( mLoc, mRadius );
    Rectf rect(mLoc.x, mLoc.y, mLoc.x+mRadius, mLoc.y+mRadius);
    gl::drawSolidRect(rect);
    gl::color(Color(1.0f,1.0f,1.0f));
    //ARROWS
    /*
     gl::color(Color(1.0f,1.0f,1.0f));
     float arrowLength = 15.0f;
     Vec3f p1 (mLoc, 0.0f);
     Vec3f p2 (mLoc+mDirToCursor*arrowLength, 0.0f);
     float headLength = 6.0f;
     float headRadius = 3.0f;
     gl::drawVector(p1,p2,headLength,headRadius); */
    //gl::drawVector(Vec3f(mLoc, 0.0f), Vec3f(mLoc+mDirToCursor*15.0f, 0.0f), 6.0f, 3.0f);
    
    //gl::drawSolidCircle(mLoc+mDirToCursor*.2f, mRadius);
}
