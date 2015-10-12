#pragma once
/// A primitive object corresponds to a mathematical primitive of some type, such as a sphere, box etc. This class is just an abstract class for such primitives.
/// Common behaviour between primitives should be added in this class.
/// A primitive object takes a real number type T to determine precision of the numbers.
template <typename T> class Primitive {
public:
	//Virtual destructor for abstract
	virtual ~Primitive() {};
	virtual T compute_area() = 0;
	virtual T compute_volume() = 0;

protected:
	//Cannot create instance of an unknown primitive object
	Primitive() = default;
};