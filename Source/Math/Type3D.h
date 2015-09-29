#pragma once
#include <ostream>
#include <GLM\glm.hpp>
#include <GLM\gtc\type_precision.hpp>
///
/// Abstract class Type3D is useful for wrapping vectors of three dimensions to a strong type. 
/// In our application it is used for vectors that are present in different coordinate systems.
/// Date: 2015-09-22
/// Author: Andreas Larsson
///
class Type3D {
public:

	virtual ~Type3D() = default;

	//Every Type3D holds an underlying double precision vector
	typedef glm::highp_dvec3 dvec_t; 

	//Implicit conversion from the underlying vector type
	virtual operator dvec_t() { return _v; } 

	//Call the print from derived class, friend because we can access print function directly
	friend std::ostream& operator<<(std::ostream &os, const Type3D &type3d) { type3d.print(os); return os; } 

	//Get the underlying vector, for neatness and safety
	const dvec_t &get() const { return _v; } 

protected:
	//Derivations can be printed differently
	virtual void print(std::ostream &os) const = 0; 

	//Initializes to zero values
	Type3D(double u = 0.0, double v = 0.0, double w = 0.0) : _v(u, v, w) {} 

	//Enables assignment of underlying vector type
	Type3D(dvec_t v) : _v(v) {}

	//The underlying vector type
	dvec_t _v; 
};