#pragma once

#include <cmath>

struct Vector4{

  double t, x, y, z;

  double Norm4(){
    return sqrt(t*t - x*x - y*y - z*z);
  }

  double Norm3(){
    return sqrt(x*x + y*y + z*z);
  }

  double Norm2(){
    return sqrt(x*x + y*y);
  }

  double Theta(){
    return atan2(Norm2(), z);
  }

  double Phi(){
    return atan2(y, x);
  }

  double Rapidity(){
    return 0.5*log((t + z)/(t - z));
  }

  double PseudoRapidity(){
    return 0.5*log((Norm3() + z)/(Norm3() - z));
  }

};

static std::istream& operator >> (std::istream& In, Vector4& V){
  In >> V.x >> V.y >> V.z >> V.t;
  return In;
}