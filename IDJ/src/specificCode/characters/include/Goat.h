/*
 * Goat.h
 *
 *  Created on: Jun 12, 2014
 *      Author: arthur
 */

#ifndef GOAT_H_
#define GOAT_H_

#include "../../../Engine.h"
#include "../../items/include/Item.h"

enum GoatState { MOV_RIGHT, MOV_LEFT, STAND_RIGHT, STAND_LEFT };

class Goat: public GameObject
{
  public:
	Goat(float x, float y);
	void Update(float dt);
	void UpdateState();
	void Render();
	void NotifyCollision(GameObject& other);
	bool IsDead();
	bool Is(std::string type);
	void Move(float dt);
  private:
	void Die();

	Sprite spWalk, spStand;
	Timer timer;
	GoatState goatState;
	int hp;

	const float MAX_TIME = 4;
	const int DAMAGE = 9000;
	const int VEL = 50;
};



#endif /* GOAT_H_ */
