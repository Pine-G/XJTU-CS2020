#include "Camera.h"

Camera::Camera() { updateCamera(); };
Camera::~Camera() {}

glm::mat4 Camera::getViewMatrix()
{
    return this->lookAt(eye, at, up);
}

glm::mat4 Camera::getProjectionMatrix()
{
    if (this-> cam_mode == 1) {
        return this->ortho(-scale, scale, -scale, scale, this->zNear, this->zFar);
    }
    else {
        return this->perspective(fov, aspect, this->zNear, this->zFar);
    }
}

glm::mat4 Camera::lookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
    // ================@TODO: Task1 补全相机观察矩阵的计算====================
    // use glm.
    // compute directly.
    // 获得相机方向。
    glm::vec4 n = glm::normalize(eye - at); 
    
    // 获得右(x)轴方向。
    glm::vec3 up_3 = up;
    glm::vec3 n_3 = n;
    glm::vec4 u = glm::normalize(glm::vec4(glm::cross(up_3,n_3),0.0));
    
    // 获得上(y)轴方向。
    glm::vec3 u_3 = u;
    glm::vec4 v = glm::normalize(glm::vec4(glm::cross(n_3,u_3), 0.0));
    
    glm::vec4 t = glm::vec4(0.0, 0.0, 0.0, 1.0);
    glm::mat4 c = glm::mat4(u, v, n, t); // 这一行需要修改
    c = glm::transpose(c);

    // 处理相机位置向量。
    glm::mat4 p = glm::mat4(1.0f);
    p[0].w = -eye[0];
    p[1].w = -eye[1];
    p[2].w = -eye[2];
    p = glm::transpose(p);

    glm::mat4 view = c * p;
    return view;                            // 计算最后需要沿-eye方向平移
}

glm::mat4 Camera::ortho(const GLfloat left, const GLfloat right,
    const GLfloat bottom, const GLfloat top,
    const GLfloat zNear, const GLfloat zFar)
{
    // =============@TODO: Task1 补全正交投影矩阵的计算=================
    glm::mat4 c = glm::mat4(1.0f);
    c[0][0] = 2.0 / (right - left);
    c[1][1] = 2.0 / (top - bottom);
    c[2][2] = 2.0 / (zNear - zFar);
    c[3][3] = 1.0;
    c[0][3] = -(right + left) / (right - left);
    c[1][3] = -(top + bottom) / (top - bottom);
    c[2][3] = -(zFar + zNear) / (zFar - zNear);
    c = glm::transpose(c);
    return c;
}

glm::mat4 Camera::perspective(const GLfloat fov, const GLfloat aspect,
    const GLfloat zNear, const GLfloat zFar)
{
    //================@TODO: Task1 补全透视投影矩阵的计算===============
    GLfloat top = tan(fov * M_PI / 180 / 2) * zNear;
    GLfloat right = top * aspect;

    glm::mat4 c = glm::mat4(1.0f);
    c[0][0] = zNear / right;
    c[1][1] = zNear / top;
    c[2][2] = (zNear + zFar) / (zNear - zFar);
    c[2][3] = 2.0 * zNear * zFar / (zNear - zFar);
    c[3][2] = -1.0;
    c[3][3] = 0.0;
    c = glm::transpose(c);
    return c;
}

glm::mat4 Camera::frustum(const GLfloat left, const GLfloat right,
    const GLfloat bottom, const GLfloat top,
    const GLfloat zNear, const GLfloat zFar)
{
    // 任意视锥体矩阵
    glm::mat4 c = glm::mat4(1.0f);
    c[0][0] = 2.0 * zNear / (right - left);
    c[0][2] = (right + left) / (right - left);
    c[1][1] = 2.0 * zNear / (top - bottom);
    c[1][2] = (top + bottom) / (top - bottom);
    c[2][2] = -(zFar + zNear) / (zFar - zNear);
    c[2][3] = -2.0 * zFar * zNear / (zFar - zNear);
    c[3][2] = -1.0;
    c[3][3] = 0.0;
    c = glm::transpose(c);
    return c;
}


void Camera::updateCamera()
{
    //================== @TODO: Task1 设置相机位置和方向 ==================
    float eyex = radius * cos(upAngle * M_PI / 180) * sin(rotateAngle * M_PI / 180);
    float eyey = radius * sin(upAngle * M_PI / 180);
    float eyez = radius * cos(upAngle * M_PI / 180) * cos(rotateAngle * M_PI / 180);

    eye = glm::vec4(eyex, eyey, eyez, 0.0);
    at = glm::vec4(0.0, 0.0, 0.0, 0.0);
    up = glm::vec4(0.0, 1.0, 0.0, 0.0);


    // 使用相对于at的角度控制相机的时候，注意在upAngle大于90的时候，相机坐标系的u向量会变成相反的方向，
    // 要将up的y轴改为负方向才不会发生这种问题
    // 比如以下改法
    
    if (upAngle > 90){
        up.y = -1;
    }
    else if (upAngle < -90){
        up.y = -1;
    }
    // 也可以考虑直接控制相机自身的俯仰角，
    // 保存up，eye-at 这些向量，并修改这些向量方向来控制
    // 看到这里的同学感兴趣可以试一试

}


void Camera::keyboard(int key, int action, int mode)
{
    if (key == GLFW_KEY_U /* && action == GLFW_PRESS */ && mode == 0x0000)
    {
        rotateAngle += 5.0;
    }
    else if (key == GLFW_KEY_U /* && action == GLFW_PRESS */ && mode == GLFW_MOD_SHIFT)
    {
        rotateAngle -= 5.0;
    }
    else if (key == GLFW_KEY_I /* && action == GLFW_PRESS */ && mode == 0x0000)
    {
        upAngle += 5.0;
        if (upAngle == 180)
            upAngle = -180;
    }
    else if (key == GLFW_KEY_I /* && action == GLFW_PRESS */ && mode == GLFW_MOD_SHIFT)
    {
        upAngle -= 5.0;
        if (upAngle == -180)
            upAngle = 180;
    }
    else if (key == GLFW_KEY_O /* && action == GLFW_PRESS */ && mode == 0x0000)
    {
        radius += 0.1;
    }
    else if (key == GLFW_KEY_O /* && action == GLFW_PRESS */ && mode == GLFW_MOD_SHIFT)
    {
        radius -= 0.1;
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && mode == 0x0000)
    {
        radius = 4.0;
        rotateAngle = 0.0;
        upAngle = 0.0;
        fov = 45.0;
        aspect = 1.0;
        scale = 1.5;
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS && mode == 0x0000)
    {
        cam_mode = 0;
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
    {
        cam_mode = 1;
    }
}
