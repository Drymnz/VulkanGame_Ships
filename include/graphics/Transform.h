#ifndef TRANSFORM_H
#define TRANSFORM_H

class Transform {
public:
    Transform();

    void setPosition(float x, float y, float z);
    void setRotation(float x, float y, float z);
    void setScale(float x, float y, float z);

    void getModelMatrix(float* matrix) const;

    const float* getPosition() const { return position; }
    const float* getRotation() const { return rotation; }
    const float* getScale() const { return scale; }

private:
    float position[3];
    float rotation[3];
    float scale[3];
};

#endif // TRANSFORM_H