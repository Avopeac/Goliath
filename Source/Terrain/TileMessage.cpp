#include "TileMessage.h"

void TileMessage::process() {
	_tile = std::make_shared<Tile>();
	_tile->generate(_resolution, _height_function, _rotation, _translation, _scale, _normalize, _gen_normals);
}

std::shared_ptr<Tile> TileMessage::get_tile() {
	return _tile;
}