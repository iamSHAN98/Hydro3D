#pragma once

#include <cmath>
#include <istream>


struct Vector4{
  double t, x, y, z;

  double Norm4(){ return sqrt(t*t - x*x - y*y - z*z); }
  double Norm3(){ return sqrt(x*x + y*y + z*z); }
  double Norm2(){ return sqrt(x*x + y*y); }

  double Theta(){ return atan2(Norm2(), z); }
  double Phi()	{ return atan2(y, x); }

  double Eta(){ return 0.5*log((Norm3() + z)/(Norm3() - z)); }
  double Rap(){ return 0.5*log((t + z)/(t - z)); }
};

inline std::istream& operator >> (std::istream& In, Vector4& V){
  In >> V.x >> V.y >> V.z >> V.t;
  return In;
}


struct Event{
	double b, PsiRP;
	int Mult, NPart;
};


struct Particle{
  int ID;
  double Mass;
  Vector4 Position, Momentum;
};
