/*
 * Wolf.cpp
 *
 *  Created on: Jun 29, 2014
 *      Author: Gabriel Naves
 */

#include "../include/Wolf.h"

/**
 * Constructor for the wolf. The wolf npc will
 * rest indefinitely. If a character comes within
 * its field of view, it gets up and attacks him.
 */
Wolf::Wolf(float x, float y, float visionDistance, bool facingRight) :
             restSp("img/characters/wolfwalk(1).png", 5, 0.1, true),
             runSp("img/characters/wolfwalk.png", 4, 0.1, true),
             attackSp("img/characters/wolfattack.png", 5, 0.1, true),
             walkSp("img/characters/wolfwalk.png", 4, 0.1, true),
             getUpSp("img/characters/wolfattack.png", 5, 0.1, false),
             lieDownSp("img/characters/wolfattack.png", 5, 0.1, false),
             heldSp("img/characters/wolfHeld.png", 5, 0.1, false)
{
    box.Set(x-restSp.GetWidth()/2, y-restSp.GetHeight()/2, restSp.GetWidth(), restSp.GetHeight());
    rotation = 0;
    hp = 1;
    this->facingRight = initialSideRight = facingRight;
    dealtDamage = false;
    state = WolfNamespace::RESTING;
    visionField.Set(box.GetX(), (box.GetY()+box.GetH())-110, visionDistance, 110);
    initialPos.Set(x, y);
}

Wolf::~Wolf() {}

void Wolf::Update(float dt)
{
    CheckIfWolfCanBeHeld();
    CheckIfBeingHeld();
    if (state == WolfNamespace::RESTING) Rest(dt);
    else if (state == WolfNamespace::RUNNING) Run(dt);
    else if (state == WolfNamespace::ATTACKING) Attack(dt);
    else if (state == WolfNamespace::RETURNING) Return(dt);
    else if (state == WolfNamespace::GET_UP) GetUp(dt);
    else if (state == WolfNamespace::LIE_DOWN) LieDown(dt);
    else if (state == WolfNamespace::BEING_HELD) BeHeld(dt);
}

void Wolf::CheckIfWolfCanBeHeld()
{
    Thor* thor = Thor::characterThor;
    if (thor == NULL)
        return;
    else if (thor->GetMovementType() != "Regular" or thor->GetVState() != STANDING or
            !visionField.IsInside(thor->box.Center()))
        return;
    else if ((state == WolfNamespace::ATTACKING or state == WolfNamespace::RUNNING) and
                abs(thor->box.Center().GetX() - box.Center().GetX() <= WolfNamespace::HOLD_DISTANCE))
    {
        if (facingRight and thor->GetHState() == (STANDING_LEFT | MOVING_LEFT))
            thor->canHoldWolf = true;
        else if (!facingRight and thor->GetHState() == (STANDING_RIGHT | MOVING_RIGHT))
            thor->canHoldWolf = true;
    }
}

/**
 * Checks if Thor is holding the wolf. If the wolf is being
 * held by Thor, changes its state to BEING_HELD
 */
void Wolf::CheckIfBeingHeld()
{
    if (Thor::characterThor == NULL)
    {
        if (state == WolfNamespace::BEING_HELD) state = WolfNamespace::RUNNING;
    }
    else if (Thor::characterThor->GetMovementType() == "HoldingWolf")
        state = WolfNamespace::BEING_HELD;
    else
    {
        if (state == WolfNamespace::BEING_HELD) state = WolfNamespace::RUNNING;
        heldSp.SetFrame(1);
        heldTimer.Restart();
    }
}

void Wolf::Rest(float dt)
{
    Rect target(FindClosestCharacter());
    restSp.Update(dt);
    // If the closest character is within the wolf's field of view, run towards it
    if (Collision::IsColliding(target, visionField, 0, 0))
    {
        state = WolfNamespace::GET_UP;
        runSp.SetFrame(1);
    }
}

void Wolf::GetUp(float dt)
{
    getUpTimer.Update(dt);
    getUpSp.Update(dt);
    if (getUpTimer.Get() >= 0.5)
    {
        getUpTimer.Restart();
        getUpSp.SetFrame(1);
        state = WolfNamespace::RUNNING;
    }
}

void Wolf::Run(float dt)
{
    Rect target(FindClosestCharacter());
    if (!Collision::IsColliding(target, visionField, 0, 0))
    {
        state = WolfNamespace::RETURNING;
        walkSp.SetFrame(1);
        return;
    }
    runSp.Update(dt);
    float distance = target.Center().GetX() - box.Center().GetX();
    float signal = (distance >= 0 ? 1 : -1);
    facingRight = (signal == 1 ? true : false);
    if (abs(distance) >= 60)
        box.MoveRect(dt*WolfNamespace::RUN_SPEED*signal, 0);
    else state = WolfNamespace::ATTACKING;
}

void Wolf::Attack(float dt)
{
    attackSp.Update(dt);
    attackTimer.Update(dt);
    if (attackTimer.Get() >= 0.2)
        box.MoveRect((facingRight ? 1 : -1)*dt*WolfNamespace::ATTACK_SPEED, 0);
    if (attackSp.GetCurrentFrame() == 4 and !dealtDamage and
            Thor::characterThor != NULL and Loki::characterLoki != NULL)
    {
        Character* target = NULL;
        if (Loki::characterLoki->box.Center().Distance(box.Center()) <
                Thor::characterThor->box.Center().Distance(box.Center()))
            target = Loki::characterLoki;
        else target = Thor::characterThor;
        if (Collision::IsColliding(target->box, box, target->rotation*2*M_PI/360, 0))
            target->DealDamage(10);
        dealtDamage = true;
    }
    if (attackTimer.Get() >= 0.5)
    {
        state = WolfNamespace::RUNNING;
        attackSp.SetFrame(1);
        attackTimer.Restart();
        dealtDamage = false;
    }
}

void Wolf::Return(float dt)
{
    Rect target(FindClosestCharacter());
    walkSp.Update(dt);
    if (Collision::IsColliding(target, visionField, 0, 0))
    {
        state = WolfNamespace::RUNNING;
        runSp.SetFrame(1);
        return;
    }
    float signal = (box.Center().GetX()-initialPos.GetX() >= 0 ? -1 : 1);
    facingRight = (signal > 0 ? true : false);
    if (box.Center().Equals(initialPos))
    {
        box.Set(initialPos.GetX()-box.GetW()/2, initialPos.GetY()-box.GetH()/2, box.GetW(), box.GetH());
        state = WolfNamespace::LIE_DOWN;
        facingRight = initialSideRight;
    }
    else box.MoveRect(dt*WolfNamespace::WALK_SPEED*signal, 0);
}

void Wolf::LieDown(float dt)
{
    lieDownTimer.Update(dt);
    lieDownSp.Update(dt);
    if (lieDownTimer.Get() >= 0.5)
    {
        lieDownTimer.Restart();
        lieDownSp.SetFrame(1);
        state = WolfNamespace::RESTING;
    }
}

void Wolf::BeHeld(float dt)
{
    heldSp.Update(dt);
    heldTimer.Update(dt);
    if (heldTimer.Get() <= 0.5 and heldTimer.Get() > 0.2)
        box.MoveRect(WolfNamespace::HOLD_SPEED*dt, 0);
}

Rect Wolf::FindClosestCharacter()
{
    if (Loki::characterLoki == NULL or Thor::characterThor == NULL) return Rect();
    Rect loki = Loki::characterLoki->box;
    Rect thor = Thor::characterThor->box;
    float lokiDistance, thorDistance;
    if (Collision::IsColliding(loki, visionField, 0, 0))
        lokiDistance = loki.Center().Distance(box.Center());
    else lokiDistance = 9999999;
    if (Collision::IsColliding(thor, visionField, 0, 0))
        thorDistance = thor.Center().Distance(box.Center());
    else thorDistance = 9999999;
    if (thorDistance < lokiDistance) return thor;
    return loki;
}

void Wolf::Render()
{
    switch (state)
    {
        case WolfNamespace::RESTING:
            restSp.Render(box.GetX()-Camera::pos.GetX(), box.GetY()-Camera::pos.GetY(), rotation*2*M_PI/360,
                            (facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            break;
        case WolfNamespace::GET_UP:
            getUpSp.Render(box.GetX()-Camera::pos.GetX(), box.GetY()-Camera::pos.GetY(), rotation*2*M_PI/360,
                    (facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            break;
        case WolfNamespace::RUNNING:
            runSp.Render(box.GetX()-Camera::pos.GetX(), box.GetY()-Camera::pos.GetY(), rotation*2*M_PI/360,
                           (facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            break;
        case WolfNamespace::ATTACKING:
            attackSp.Render(box.GetX()-Camera::pos.GetX(), box.GetY()-Camera::pos.GetY(), rotation*2*M_PI/360,
                             (facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            break;
        case WolfNamespace::RETURNING:
            walkSp.Render(box.GetX()-Camera::pos.GetX(), box.GetY()-Camera::pos.GetY(), rotation*2*M_PI/360,
                    (facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            break;
        case WolfNamespace::LIE_DOWN:
            lieDownSp.Render(box.GetX()-Camera::pos.GetX(), box.GetY()-Camera::pos.GetY(), rotation*2*M_PI/360,
                    (facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            break;
        case WolfNamespace::BEING_HELD:
            heldSp.Render(box.GetX()-Camera::pos.GetX(), box.GetY()-Camera::pos.GetY(), rotation*2*M_PI/360,
                    (facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            break;
    }
}

void Wolf::NotifyCollision(GameObject& other)
{
    if (other.Is("LokiBullet")) hp -= 1;
}

bool Wolf::IsDead()
{
    return hp <= 0 ? true : false;
}

bool Wolf::Is(std::string type)
{
    return type == "Wolf" ? true : false;
}
