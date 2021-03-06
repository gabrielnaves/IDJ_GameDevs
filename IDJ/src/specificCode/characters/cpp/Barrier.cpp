/*
 * Barrier.cpp
 *
 *  Created on: May 22, 2014
 *      Author: Icaro Mota
 *              Gabriel Naves
 */

#include "../include/Barrier.h"

Barrier* Barrier::barrier;

Barrier::Barrier() : sp("img/characters/barreira.png")
{
	box.Set(GetCorner(),DIAMETER,DIAMETER);
	rotation = 0;
	barrier = this;
}

Barrier::~Barrier()
{
	barrier = NULL;
}

/**
 * Gets the corner of the box based on the coordinate of the center of the barrier
 */
Point Barrier::GetCorner()
{
	return (Point(FindCenter().GetX() - DIAMETER/2, FindCenter().GetY() - DIAMETER/2));
}

/**
 * Find the center of the barrier, which is located in the middle of the 2 characters
 */
Point Barrier::FindCenter()
{
	Point thor = Thor::characterThor->box.Center();
	Point loki = Loki::characterLoki->box.Center();
	Point center;

	float Y, y; //Y > y
	float X, x; //X > x

	//sees where is the greater Y value
	if (thor.GetY() > loki.GetY())
		Y = thor.GetY(), y = loki.GetY();
	else
		Y = loki.GetY(), y = thor.GetY();
	//sees where is the greater X value
	if (thor.GetX() > loki.GetX())
		X = thor.GetX(), x = loki.GetX();
	else
		X = loki.GetX(), x = thor.GetX();

	center.Set((X-x)/2 + x,(Y-y)/2 + y);
	return(center);
}

/**
 * Updates the position of the barrier
 */
void Barrier::Update(float dt)
{
	box.SetPoint(GetCorner());
}

void Barrier::Render()
{
//	if (Thor::characterThor->box.Center().Distance(Loki::characterLoki->box.Center()) > DIAMETER)
//		sp.Render(box.GetX()-Camera::pos.GetX(), box.GetY()-Camera::pos.GetY());
}
/**
 * Should not have to colide with anything
 */
void Barrier::NotifyCollision(GameObject& other){}

/**
 * The barrier dies when any of the characters die
 */
bool Barrier::IsDead()
{
	return (Thor::characterThor == NULL or Loki::characterLoki == NULL);
}

bool Barrier::Is(std::string type)
{
	return type == "Barrier" ? true : false;
}

/**
 * Does not allow the characters to have a distance bigger than DIAMETER in any direction
 */
void Barrier::CheckCollision(Character* character)
{
    Character* other = NULL;
    float distance;
    if (character->Is("Loki")) other = Thor::characterThor;
    else other = Loki::characterLoki;
    distance = character->box.Center().Distance(other->box.Center());
    if (distance > DIAMETER + DRAG_TOLERANCE)
    {
    	if (character->barrierSuspended)
    		character->box.MoveRectWithAngle(distance - DIAMETER, character->box.Center().FindAngle(other->box.Center()));
    	else if (other->barrierSuspended)
    		other->box.MoveRectWithAngle(distance - DIAMETER, other->box.Center().FindAngle(character->box.Center()));
    	else
    		character->box.MoveRectWithAngle(distance - DIAMETER, character->box.Center().FindAngle(other->box.Center()));
    }
}
