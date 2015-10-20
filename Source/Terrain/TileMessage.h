#pragma once
#include <memory>
#include "Message.h"
#include "..\QuadTree\HeightFunction.h"
#include "..\QuadTree\Tile.h"
///A tile message is a subclass of message, it contains the information to generate a tile object. This message can be sent to the job scheduler and some thread will do the work.
class TileMessage : public Message {
public:
	//The constructor takes all the parameters needed for generation of a tile object
	TileMessage(int resolution, std::shared_ptr<HeightFunction> height_function, const glm::dmat4 &rotation, const glm::dvec3 &translation, const glm::dmat4 &scale, bool normalize, bool gen_normals)
		: Message(), _resolution(resolution), _height_function(height_function), _rotation(rotation), _translation(translation), _scale(scale), _normalize(normalize), _gen_normals(gen_normals) {}
	~TileMessage() {}
	//Get the tile object
	std::shared_ptr<Tile> get_tile();
	//Inherited from Message, will generate the tile
	void process() override;

private:
	//Tile resolution in vertices in one dimension
	int _resolution;
	//The given height function for the tile generation
	std::shared_ptr<HeightFunction> _height_function;
	//Vertex transformations
	const glm::dmat4 _rotation;
	const glm::dvec3 _translation;
	const glm::dmat4 _scale;
	//Mesh normalization
	bool _normalize;
	//Generate normals on generation
	bool _gen_normals;
	//The tile object that will be generated
	std::shared_ptr<Tile> _tile;
};