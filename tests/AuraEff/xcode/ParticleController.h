//
//  ParticleController.h
//  AuraEff
//
//  Created by Jose D Leal on 6/21/13.
//
//
#pragma once
#include "cinder/Channel.h"
#include "Particle.h"
#include <list>

class ParticleController {
public:
	ParticleController();
	ParticleController( int res );
	void update( const ci::Channel32f &channel, const ci::Vec2i &mouseLoc );
	void draw();
	void addParticle( int xRes, int yRes, int res );
	void addParticles( int amt );
	void removeParticles( int amt );
	
	std::list<Particle>	mParticles;
	
	int mXRes, mYRes;
};
#ifndef AuraEff_ParticleController_h
#define AuraEff_ParticleController_h



#endif
