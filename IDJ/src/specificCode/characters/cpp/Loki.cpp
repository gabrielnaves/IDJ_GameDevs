/*
 * Loki.cpp
 *
 *  Created on: May 19, 2014
 *      Author: Gabriel Naves
 *              Icaro Mota
 */

#include "../include/Loki.h"

Loki* Loki::characterLoki;

Loki::Loki(float x, float y, MovementMap& movMap) : Character(movMap)
{
	rotation = 0;
	tempCharacterSp.Open("img/Characters/tempLoki.jpg");
	box.Set(x-tempCharacterSp.GetWidth()/2, y-tempCharacterSp.GetHeight()/2, tempCharacterSp.GetWidth(), tempCharacterSp.GetHeight());
	characterLoki = this;
	appearance = LOKI;
	flappedWings = 0;
	TIMES_FLAPS_WINGS = 1;
	actionState = NONE;
}

Loki::~Loki()
{
	characterLoki = NULL;
}

void Loki::Input()
{
    horizontal = vertical = 0;
    InputManager &input = InputManager::GetInstance();
    //Gets the inputs for moving horizontally
    if (input.KeyPress(SDLK_d) || input.IsKeyDown(SDLK_d))
        horizontal += 1;
    if (input.KeyPress(SDLK_a) || input.IsKeyDown(SDLK_a))
        horizontal -= 1;
    //Gets the inputs for moving vertically
    if (input.KeyPress(SDLK_w))
        vertical += 1;
    else if (input.IsKeyDown(SDLK_w))
        vertical = 2;
    if (input.KeyPress(SDLK_s) || input.IsKeyDown(SDLK_s))
        vertical -= 1;
    //Action button
    if (input.KeyPress(SDLK_e))
        actionButton = true;
}

void Loki::Shoot()
{
    Sprite spBullet("img/Characters/minionbullet.png",3,0.1);
    float shootingAngle = (hState == MOVING_RIGHT or hState == STANDING_RIGHT) ? 0 : M_PI;
    Bullet* fireBall=new Bullet(box.Center().GetX(),box.Center().GetY(),shootingAngle,FIREBALL_SPEED,FIREBALL_DISTANCE,spBullet,"Loki");
    Game::GetInstance().GetCurrentState().AddObject(fireBall); //add the bullet to the objectArray

    shootCooldown.Restart(); //restart the timer
    actionState = NONE;
}

void Loki::UpdateEagleSpeed(float dt)
{
    if (vState == JUST_JUMPED)
        speed.Set(speed.GetX(),EAGLE_JUMP_SPEED);
    else if (vState == FALLING or vState == JUMPING) //unnecessary if. it is here only in case more vStates are implemented
        speed = speed + Point(speed.GetX(),(GRAVITY - EAGLE_AIR_RESISTANCE)*dt);

    if (hState == STANDING_LEFT or hState == STANDING_RIGHT) speed.Set(0,speed.GetY());
    else if (hState == MOVING_RIGHT) speed.Set(VEL,speed.GetY());
    else if (hState == MOVING_LEFT) speed.Set(-VEL,speed.GetY());

    if (speed.GetY() >= MAX_FALLING_SPEED_EAGLE) speed.Set(speed.GetX(), MAX_FALLING_SPEED_EAGLE);
}

void Loki::UpdateVerticalState()
{
    if (appearance == LOKI)
    {
        if (vertical == 1 and vState == STANDING)
            vState = JUST_JUMPED;
        else if (vertical == 1 and vState == FALLING and actionState == NONE)
            appearance = EAGLE;
    }
    else if (appearance == EAGLE)
    {
        if (vertical == 1 and flappedWings < TIMES_FLAPS_WINGS){
            vState = JUST_JUMPED;
            flappedWings++;
        }
        else if (vertical == -1)
            appearance = LOKI;
    }
}

void Loki::Move(float dt)
{
    if (actionState == CLIMBING)
        Climb(dt);
    else if (appearance == LOKI) UpdateSpeed(dt);
    else if (appearance == EAGLE) UpdateEagleSpeed(dt);

    box.MoveRect(speed.GetX()*dt,speed.GetY()*dt);
    Barrier::barrier->CheckCollision(this);

    //If the character colided with the upper limit of the barrier, makes sure it will not keep on jumping
    if ((vState == JUMPING or vState == JUST_JUMPED) and (Thor::characterThor->box.GetY() - box.GetY() >= Barrier::barrier->DIAMETER))
        speed.Set(speed.GetX(),0); //sets the Y speed to 0 when it hits the upper limmit of the barrier
}

/**
 * Calls the right action Loki must do, depending on the situation
 */
void Loki::Act()
{
    if (appearance == LOKI)
    {
        //releases the stairs
        if (actionState == CLIMBING)
        {
            actionState = NONE;
            vState = FALLING;
        }
        //holds the stairs
        else if (canHoldStairs)
        {
            actionState = CLIMBING;
            box.SetPoint(box.GetPoint().GetX(),box.GetPoint().GetY()+1);
        }
        //Shoot
        else if(shootCooldown.Get() >= COOLDOWN and vState == STANDING)
            Shoot();
    }
    actionButton = false;
}

void Loki::UpdatesStateOnTheFall()
{
    if (vState == JUST_JUMPED)
        vState = JUMPING;
    if (vState == STANDING)
    {
        appearance = LOKI;
        flappedWings = 0;
    }
    else if (speed.GetY()>0) vState = FALLING;
}

void Loki::Update(float dt)
{
    Input();
    if (actionButton) Act();
    shootCooldown.Update(dt);
    UpdateState();
    if (actionState == CLIMBING and !canHoldStairs) actionState = NONE;
    Move(dt);
    UpdatesStateOnTheFall();
    UpdateSprite();
    CheckMovementLimits();
}

void Loki::Render()
{
	tempCharacterSp.Render(box.GetX()-Camera::pos.GetX(), box.GetY()-Camera::pos.GetY());
}

void Loki::NotifyCollision(GameObject& other)
{
    if (other.Is("Stairs"))
    {
        if ((box.GetX() >= box.GetX()) and ((box.GetX()+box.GetW()) <= (other.box.GetX()+other.box.GetW())))
        {
                //if the character is close enough in the y axis to go down the stairs
                if (box.GetY()<=(other.box.GetY()-box.GetH()+50) and box.GetY()>=(other.box.GetY()-box.GetH()-2) and actionState != CLIMBING)
                {
                    box.SetPoint(box.GetX(),other.box.GetPoint().GetY()-box.GetH()); //corrects bugs
                    vState = STANDING;
                }
        }
    }
    if (other.Is("Spikes"))
        hp -= HP/20;
}

bool Loki::Is(std::string type)
{
	return type == "Loki" ? true : false;
}

/**
 * Updates the sprite based on the state of the character
 */
void Loki::UpdateSprite()
{
    if (appearance == LOKI)
    {
        if (hState == MOVING_RIGHT)
            tempCharacterSp.Open("img/Characters/tempLokiInvertido.png");
        if (hState == STANDING_RIGHT)
            tempCharacterSp.Open("img/Characters/tempLokiInvertido.png");
        if (hState == MOVING_LEFT)
            tempCharacterSp.Open("img/Characters/tempLoki.jpg");
        if (hState == STANDING_LEFT)
            tempCharacterSp.Open("img/Characters/tempLoki.jpg");
    }
    else if (appearance == EAGLE)
    {
        tempCharacterSp.Open("img/Characters/aguia.png");
    }
}

bool Loki::IsClimbing()
{
    return (actionState == CLIMBING ? true : false);
}
