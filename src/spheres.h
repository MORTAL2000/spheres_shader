/*****************************************************************************/
/**
 * @file spheres.h
 * @brief Spheres rendering interface used for compile-time polymorphism.
 * @author Matthias Werner
 * @sa http://11235813tdd.blogspot.de/
 * @date 2016/03/12: Initial commit.
 *****************************************************************************/

#ifndef SPHERES_H_
#define SPHERES_H_

#include "camera.h"
#include <assert.h>
#include <glm/glm.hpp>
#include <string>

/**
 * Template sphere rendering class. It defines the interface for sphere
 * rendering implementations and also wraps them with a simple test, whether renderer
 * has been created or not.
 */
template<typename TSpheres, unsigned TNumSpheres>
class Spheres
{
  public:
    Spheres() : _created(0) {}

    const std::string getDescription() const{
      return static_cast<TSpheres*>(this)->getDescription();
    }

    int create(float radius_mean, float radius_var){
      assert(_created==0);
      return static_cast<TSpheres*>(this)->create(radius_mean, radius_var);
      _created=1;
    }

    int recompile(){
      assert(_created==1);
      return static_cast<TSpheres*>(this)->recompile();
    }

    void bind(const float* lightPos, const Camera& camera){
      assert(_created==1);
      static_cast<TSpheres*>(this)->bind(lightPos, camera);
    }

    void operator()(){
      assert(_created==1);
      static_cast<TSpheres*>(this)();
    }

    void unbind(){
      assert(_created==1);
      static_cast<TSpheres*>(this)->unbind();
    }

    void cleanup(){
      assert(_created==1);
      static_cast<TSpheres*>(this)->cleanup();
    }

  private:
    int _created;
};


#endif /* SPHERES_H_ */
