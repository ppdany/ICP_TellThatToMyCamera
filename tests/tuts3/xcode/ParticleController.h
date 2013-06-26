//
//  ParticleController.h
//  tuts2.5 - cinder tutorial special test
//
//  Created by Jose D Leal on 6/22/13.
//
//
#pragma once
#include "Particle.h"
#include <list>

class ParticleController {
public:
	ParticleController();
    ParticleController(int res);
	void update(const ci::Channel32f &channel, const ci::Vec2i &mouseLoc);
	void draw();
    void addParticles (int amt);
	void addParticle( int xRes, int yRes, int res );
	void removeParticles( int amt );
	
	std::list<Particle>	mParticles;
    
    int mXRes, mYRes;
};