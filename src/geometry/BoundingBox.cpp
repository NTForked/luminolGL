//
// Created by mehdi on 16/02/16.
//

#include <glog/logging.h>
#include <stdexcept>
#include "geometry/BoundingBox.h"
#include "glm/ext.hpp"

namespace Geometry
{
    bool BoundingBox::isVisible(const glm::mat4 &VP)  const {
        glm::vec4 projInitPoint = VP * glm::vec4(_points[0], 1.0);
        projInitPoint /= projInitPoint.w;

        float xmin = projInitPoint.x;
        float xmax = projInitPoint.x;
        float ymin = projInitPoint.y;
        float ymax = projInitPoint.y;
        bool isInFrontOfCamera = false;

        for(auto& point : _points){
            glm::vec4 projPoint = VP * glm::vec4(point, 1.0);

            // no need to be in homogeneous coordinates
            if( projPoint.z > 0) isInFrontOfCamera = true;

            // better behavior with absolute W. Don't know why could be < 0.
            // Any mathematical explanations are welcome :)
            projPoint /= glm::abs(projPoint.w);

            if( projPoint.x < xmin) xmin = projPoint.x;
            if( projPoint.y > ymax) ymax = projPoint.y;
            if( projPoint.x > xmax) xmax = projPoint.x;
            if( projPoint.y < ymin) ymin = projPoint.y;
        }

        if(!isInFrontOfCamera)
            return false;

        // screen space limit (between -1 and 1 for each axis x,y and z)
        float limit = 1;
        return !(
                    (xmax<-limit) ||
                    (ymax<-limit) ||
                    (xmin>limit)  ||
                    (ymin>limit)
                );
    }


    void BoundingBox::compute(const std::vector<Graphics::VertexDescriptor> &vertices) {

        float xmin, ymin, zmin;
        float xmax, ymax, zmax;

        xmin = xmax = vertices[0].position.x;
        ymin = ymax = vertices[0].position.y;
        zmin = zmax = vertices[0].position.z;

        for(auto& vertice : vertices){
            if (vertice.position.x < xmin) xmin = vertice.position.x;
            if (vertice.position.x > xmax) xmax = vertice.position.x;
            if (vertice.position.y < ymin) ymin = vertice.position.y;
            if (vertice.position.y > ymax) ymax = vertice.position.y;
            if (vertice.position.z < zmin) zmin = vertice.position.z;
            if (vertice.position.z > zmax) zmax = vertice.position.z;
        }

        _points = {
              glm::vec3(xmin,ymin,zmin),
              glm::vec3(xmin,ymin,zmax),
              glm::vec3(xmax,ymin,zmax),
              glm::vec3(xmax,ymin,zmin),
              glm::vec3(xmin,ymax,zmin),
              glm::vec3(xmin,ymax,zmax),
              glm::vec3(xmax,ymax,zmax),
              glm::vec3(xmax,ymax,zmin)
        };

    }

    const std::vector<glm::vec3> &BoundingBox::getVector() const{
        return _points;
    }

    BoundingBox operator*(const glm::mat4 &trans, const BoundingBox& box1) {
        Geometry::BoundingBox box;

        for(auto &point : box1._points){
            glm::vec4 transPoint = trans * glm::vec4(point,1);
            box._points.push_back(glm::vec3(transPoint));
        }

        return box;
    }

    BoundingBox::BoundingBox() {
        _points.reserve(8);
    }
}

