/*
 * Level1State.cpp
 *
 *  Created on: May 19, 2014
 *      Author: Gabriel Naves
 *              Icaro Mota
 */

#include "../include/Level1State.h"

Level1State::Level1State() : State(), tileSet(55,55,"img/level1/level1Tiles.png"),
                             tileMap("map/level1.txt", &tileSet),
                             movementMap("map/level1MovementMap.txt", tileSet)
{
    bg.Open("img/level1/level1Background2.png");
    rochas.Open("img/level1/rocks.png");
    EmplaceInitialObjects();
    Follow("Barrier");
    outsideMusic = new Music("audio/SOUNDTRACK MODE/Fase 1/Fase 1 (Parte superior) - Eber Filipe.mp3");
    caveMusic = new Music("audio/SOUNDTRACK MODE/Fase 1/Subsolo Fase 1.mp3");
    outsideMusic->Play(-1);
    musicPlaying = true;
}

Level1State::~Level1State()
{
}

void Level1State::EmplaceInitialObjects()
{
    objectArray.emplace_back(new Stairs("img/objects/stairs.png",605,440));
    objectArray.emplace_back(new Stairs("img/objects/stairs.png",390,550));
    objectArray.emplace_back(new Stairs("img/level1/grade.png",380,1350, 6, 0.1));
    objectArray.emplace_back(new BrokenHouseBack(500, 320));
    objectArray.emplace_back(new Loki(70,100, movementMap));
    objectArray.emplace_back(new Thor(20,100, movementMap));
    objectArray.emplace_back(new Bridge(220, 330));
    objectArray.emplace_back(new BrokenHouseFront(500, 320));
    objectArray.emplace_back(new Item(355,520,"img/items/lokifire.png", "LokiFire", false, true));
    objectArray.emplace_back(new Spikes("img/objects/spikes.png",385,2200));
    objectArray.emplace_back(new Plants("img/level1/plants/5.png",255,510));
    objectArray.emplace_back(new Plants("img/level1/plants/3.png",260,525));
    objectArray.emplace_back(new Plants("img/level1/plants/10.png",410,530));
    objectArray.emplace_back(new Plants("img/level1/plants/9.png",390,530));
    objectArray.emplace_back(new Plants("img/level1/plants/4.png",395,515));
    objectArray.emplace_back(new Plants("img/level1/plants/1.png",395,485));
    objectArray.emplace_back(new Plants("img/level1/plants/10.png",365,530));
    objectArray.emplace_back(new Plants("img/level1/plants/6.png",365,515));
    objectArray.emplace_back(new Plants("img/level1/plants/2.png",325,525));
    objectArray.emplace_back(new Plants("img/level1/plants/7.png",285,525));
    objectArray.emplace_back(new Plants("img/level1/plants/10.png",450,425));
    objectArray.emplace_back(new Plants("img/level1/plants/5.png",425,405));
    objectArray.emplace_back(new Plants("img/level1/plants/2.png",420,430));
    objectArray.emplace_back(new Plants("img/level1/plants/3.png",425,425));
    objectArray.emplace_back(new Plants("img/level1/plants/9.png",480,425));
    objectArray.emplace_back(new Plants("img/level1/plants/10.png",500,425));
    objectArray.emplace_back(new Plants("img/level1/plants/10.png",530,425));
    objectArray.emplace_back(new Plants("img/level1/plants/4.png",530,405));
    objectArray.emplace_back(new Plants("img/level1/plants/10.png",600,420));
    objectArray.emplace_back(new Plants("img/level1/plants/6.png",570,410));
    objectArray.emplace_back(new Plants("img/level1/plants/7.png",500,415));
    objectArray.emplace_back(new Plants("img/level1/plants/1.png",460,415));
    objectArray.emplace_back(new Plants("img/level1/plants/9.png",525,320));
    objectArray.emplace_back(new Plants("img/level1/plants/10.png",530,320));
    objectArray.emplace_back(new Plants("img/level1/plants/10.png",580,375));
    objectArray.emplace_back(new Plants("img/level1/plants/9.png",640,375));
    objectArray.emplace_back(new Plants("img/level1/plants/10.png",610,375));
    objectArray.emplace_back(new Goat(800, 310));
    objectArray.emplace_back(new Wolf(60, 744, 480));
    objectArray.emplace_back(new Barrier());
}

void Level1State::Follow(std::string object)
{
    for (int i = 0; i < (int)objectArray.size(); i++)
        if (objectArray[i]->Is(object))
            Camera::Follow(&(*objectArray[i]), true, 0, 0, tileMap.GetWidth()*tileSet.GetTileWidth() - Game::GetInstance().GetWindowWidth(),
                           tileMap.GetHeight()*tileSet.GetTileHeight() - Game::GetInstance().GetWindowHeight());
}

void Level1State::Update(float dt)
{
    Input();
    if (Thor::characterThor != NULL && Loki::characterLoki != NULL && Barrier::barrier != NULL)
    {
        UpdateArray(dt);
        ChecksForCollisions();
        ErasesDeadObjects();
    }
    if (Barrier::barrier == NULL) Camera::Unfollow();
    Camera::Update(dt);
    UpdateMusic(dt);
}

void Level1State::Render()
{
    bg.Render(-Camera::pos.GetX(),-Camera::pos.GetY());
    rochas.Render(-Camera::pos.GetX(),-Camera::pos.GetY());
    tileMap.RenderLayer(0,+Camera::pos.GetX(), +Camera::pos.GetY());
    RenderArray();
    if (Loki::characterLoki != NULL && Thor::characterThor != NULL)
        if (!tileMap.HasTile(Loki::characterLoki->box.Center().GetX(), Loki::characterLoki->box.Center().GetY(), 1) &&
            !tileMap.HasTile(Thor::characterThor->box.Center().GetX(), Thor::characterThor->box.Center().GetY(), 1))
            tileMap.RenderLayer(1,+Camera::pos.GetX(), +Camera::pos.GetY());
}

void Level1State::Input()
{
    if (InputManager::GetInstance().ShouldQuit())
        requestQuit = true;
}

/*Sees if any objects collided*/
void Level1State::ChecksForCollisions()
{
    for(unsigned int i = 0; i<objectArray.size()-1; i++)
    {
        for (unsigned int j=i+1;j<objectArray.size();j++)
        {
            if (Collision::IsColliding(objectArray[j]->box, objectArray[i]->box,
                                       objectArray[j]->rotation*2*M_PI/360, objectArray[i]->rotation*2*M_PI/360))
            {
               objectArray[j]->NotifyCollision(*objectArray[i]);
               objectArray[i]->NotifyCollision(*objectArray[j]);
            }
        }
    }
}

void Level1State::ErasesDeadObjects()
{
    for (unsigned int i = 0; i < objectArray.size(); i++)
        if (objectArray[i]->IsDead())
            objectArray.erase(objectArray.begin()+i), i--;
}

void Level1State::UpdateMusic(float dt)
{
    if (Barrier::barrier == NULL) return;
    CheckMusic(605, 660, 440, 550);
    CheckMusic(385, 440, 550, 660);
}

void Level1State::CheckMusic(float lowerX, float upperX, float lowerY, float upperY)
{
    Point barrierPos(Barrier::barrier->box.Center());
    if (barrierPos.GetX() >= lowerX && barrierPos.GetX() <= upperX)
    {
        if (barrierPos.GetY() < lowerY && !musicPlaying)
        {
            delete outsideMusic;
            Resource::Clear();
            outsideMusic = new Music("audio/SOUNDTRACK MODE/Fase 1/Fase 1 (Parte superior) - Eber Filipe.mp3");
            outsideMusic->Play(-1);
            musicPlaying = true;
        }
        if (barrierPos.GetY() >= lowerY && barrierPos.GetY() <= upperY)
        {
            if (musicPlaying)
            {
                outsideMusic->Stop(0.3);
                musicPlaying = false;
            }
        }
        if (barrierPos.GetY() > upperY && !musicPlaying)
        {
            delete caveMusic;
            Resource::Clear();
            caveMusic = new Music("audio/SOUNDTRACK MODE/Fase 1/Subsolo Fase 1.mp3");
            caveMusic->Play(-1);
            musicPlaying = true;
        }
    }
}
