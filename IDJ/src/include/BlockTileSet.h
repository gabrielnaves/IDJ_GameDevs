/*
 * BlockTileSet.h
 *
 *  Created on: Apr 24, 2014
 *      Author: Gabriel Naves
 */

#ifndef BLOCKTILESET_H_
#define BLOCKTILESET_H_

#include "TileSet.h"

class BlockTileSet : public TileSet
{
  public:
    BlockTileSet(int tileWidth, int tileHeight, std::string file);
    void Open(std::string file);
    void Render(unsigned int index, int x, int y);

  private:
    Sprite tileSet;
    int rows, columns;
};

#endif /* BLOCKTILESET_H_ */
