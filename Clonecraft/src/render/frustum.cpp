#include "frustum.h"
#include "render/mesh.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>
#include <vector>

#include <iostream>

Frustum::Frustum(Camera camera, glm::mat4 projection) {
	this->camera = camera;
	this->projection = projection;

	// extract info from projection matrix
	fov = 2.0f * atan(1.0f / projection[1][1]) * 180.0f / glm::pi<float>();
	fov = glm::radians(10.0f);
	aspect_ratio = projection[1][1] / projection[0][0];
	near = projection[3][2] / (projection[2][2] - 1.0f);
	far = projection[3][2] / (projection[2][2] + 1.0f);
	//std::cout << "fov " << fov << " aspect " << aspect_ratio << " near " << near << " far " << far << std::endl;
}

// tests if point is inside frustum
// true = inside, false = outside
bool Frustum::pointTest(glm::vec3 point) {
	for (int i = FrustumPlane::TOP; i != FrustumPlane::FAR; i++) {
	//for (int i = FrustumPlane::TOP; i != FrustumPlane::BOTTOM; i++) {
		/*if (planes[i].GetPointDistance(point) < 0) {
			return false;
		}*/
		Plane plane = planes[i];
		float dist = glm::dot(plane.n, point - plane.p);
		//std::cout << "distance from point " << glm::to_string(point) << " to plane " << i << " is: " << dist << std::endl;
		if (dist > 0.0f) return true;
		//std::cout << "distance from point to plane: " << dist << std::endl;
	}
	return false;
}

void Frustum::generatePlanes() {
	// get camera vectors
	/*glm::vec3 pos = camera.getPosition();
	glm::vec3 dir = camera.getDirection();
	glm::vec3 up = camera.getUp();
	glm::vec3 right = glm::normalize(glm::cross(dir, up));*/

	glm::vec3 pos = glm::vec3(20.0f, 20.0f, 20.0f);
	glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right = glm::normalize(glm::cross(dir, up));

	// near and far coords
	glm::vec3 n_pos = pos + near * dir;
	glm::vec3 f_pos = pos + far * dir;

	// dimensions of near and far planes
	float n_height = 2 * tan(fov / 2) * near;
	float n_width = n_height * aspect_ratio;

	float f_height = 2 * tan(fov / 2) * far;
	float f_width = f_height * aspect_ratio;

	// ntl = near top left
	//ntl = n_pos + up * (n_height / 2.0f) - right * (n_width / 2.0f);
	//nbl = n_pos - up * (n_height / 2.0f) - right * (n_width / 2.0f);
	//ntr = n_pos + up * (n_height / 2.0f) + right * (n_width / 2.0f);
	//nbr = n_pos - up * (n_height / 2.0f) + right * (n_width / 2.0f);

	//ftl = f_pos + up * (f_height / 2.0f) - right * (f_width / 2.0f);
	//fbl = f_pos - up * (f_height / 2.0f) - right * (f_width / 2.0f);
	//ftr = f_pos + up * (f_height / 2.0f) + right * (f_width / 2.0f);
	//fbr = f_pos - up * (f_height / 2.0f) + right * (f_width / 2.0f);

	ntl = n_pos + up * (f_height / 2.0f) - right * (f_width / 2.0f);
	nbl = n_pos - up * (f_height / 2.0f) - right * (f_width / 2.0f);
	ntr = n_pos + up * (f_height / 2.0f) + right * (f_width / 2.0f);
	nbr = n_pos - up * (f_height / 2.0f) + right * (f_width / 2.0f);

	ftl = f_pos + up * (f_height / 2.0f) - right * (f_width / 2.0f);
	fbl = f_pos - up * (f_height / 2.0f) - right * (f_width / 2.0f);
	ftr = f_pos + up * (f_height / 2.0f) + right * (f_width / 2.0f);
	fbr = f_pos - up * (f_height / 2.0f) + right * (f_width / 2.0f);


	//std::cout << "ntl " << glm::to_string(ntl) << std::endl;
	//std::cout << "nbl " << glm::to_string(nbl) << std::endl;
	//std::cout << "ntr " << glm::to_string(ntr) << std::endl;
	//std::cout << "nbr " << glm::to_string(nbr) << std::endl;

	//std::cout << "ftl " << glm::to_string(ftl) << std::endl;
	//std::cout << "fbl " << glm::to_string(fbl) << std::endl;
	//std::cout << "ftr " << glm::to_string(ftr) << std::endl;
	//std::cout << "fbr " << glm::to_string(fbr) << std::endl;
	
	// plane normals
	glm::vec3 near_normal = glm::normalize(glm::cross(glm::vec3(nbl - nbr), glm::vec3(ntr - nbr)));
	glm::vec3 far_normal = glm::normalize(glm::cross(glm::vec3(fbr - fbl), glm::vec3(ftl - fbl)));
	glm::vec3 left_normal = glm::normalize(glm::cross(glm::vec3(ftl - fbl), glm::vec3(nbl - fbl)));
	glm::vec3 right_normal = glm::normalize(glm::cross(glm::vec3(nbr - fbr), glm::vec3(ftr - fbr)));
	glm::vec3 top_normal = glm::normalize(glm::cross(glm::vec3(ntl - ntr), glm::vec3(ftr - ntr)));
	glm::vec3 bottom_normal = glm::normalize(glm::cross(glm::vec3(fbl - fbr), glm::vec3(nbr - fbr)));

	//std::cout << "bottom_normal " << glm::to_string(bottom_normal) << std::endl;

	// construct frustum planes
	planes[FrustumPlane::NEAR] = Plane{ near_normal, nbr };
	planes[FrustumPlane::FAR] = Plane{ far_normal,  fbr };
	planes[FrustumPlane::LEFT] = Plane{ left_normal, fbl };
	planes[FrustumPlane::RIGHT] = Plane{ right_normal, fbr };
	planes[FrustumPlane::TOP] = Plane{ top_normal, ftr };
	planes[FrustumPlane::BOTTOM] = Plane{ bottom_normal, fbr };
}

// render frustum for debugging
// todo: currently very broken
void Frustum::render() {
	std::vector<GLfloat> vertices;

	for (int i = 0; i < 3; i++) {
		vertices.push_back(ntl[i]);
	}
	for (int i = 0; i < 3; i++) {
		vertices.push_back(nbl[i]);
	}
	for (int i = 0; i < 3; i++) {
		vertices.push_back(ntr[i]);
	}
	for (int i = 0; i < 3; i++) {
		vertices.push_back(nbr[i]);
	}

	for (int i = 0; i < 3; i++) {
		vertices.push_back(ftl[i]);
	}
	for (int i = 0; i < 3; i++) {
		vertices.push_back(fbl[i]);
	}
	for (int i = 0; i < 3; i++) {
		vertices.push_back(ftr[i]);
	}
	for (int i = 0; i < 3; i++) {
		vertices.push_back(fbr[i]);
	}

	// ntl 0
	// nbl 1
	// ntr 2
	// nbr 3
	// ftl 4
	// fbl 5
	// ftr 6
	// fbr 7

	/*planes[FrustumPlane::BOTTOM] = Plane{ nbl, nbr, fbr };
	planes[FrustumPlane::TOP] = Plane{ ntl, ntr, ftr };
	planes[FrustumPlane::LEFT] = Plane{ nbl, ntl, fbl };
	planes[FrustumPlane::RIGHT] = Plane{ nbr, ntr, fbr };
	planes[FrustumPlane::NEAR] = Plane{ nbl, nbr, ntr };
	planes[FrustumPlane::FAR] = Plane{ fbl, fbr, ftr };*/

	std::vector<GLuint> indices = {
		0, 1, 1, 3, 3, 2, 2, 0,	// near
		4, 5, 5, 7, 7, 6, 6, 4,	// far
		0, 4, 4, 6, 6, 2, 2, 0,	// top
		1, 2, 2, 7, 7, 3, 3, 1,	// bot
		0, 4, 4, 2, 2, 1, 1, 0,	// left
		3, 2, 2, 6, 6, 7, 7, 3,	// right
	};

	Mesh mesh(vertices, indices);
	mesh.render(GL_LINES);
	mesh.cleanup();

}