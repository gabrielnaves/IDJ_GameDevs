/*
 * Bridge.h
 *
 *  Created on: Jun 9, 2014
 *      Author: Gabriel Naves
 */

#ifndef BRIDGE_H_
#define BRIDGE_H_

#include "../../../engine/include/GameObject.h"
#include "../../../engine/include/Sprite.h"
#include "../../../engine/include/Camera.h"
#include "../../../engine/include/LineSegment.h"

class Bridge : public GameObject
{
  public:
    Bridge(float x, float y);
    ~Bridge();

    void Update(float dt);
    void Render();
    void NotifyCollision(GameObject& other);

    bool IsDead();
    bool Is(std::string type);

  private:
    Sprite bridgeSp;
    LineSegment segment1, segment2, segment3;
};

#endif /* BRIDGE_H_ */
