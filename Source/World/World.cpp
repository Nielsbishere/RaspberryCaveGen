#include "../../Headers/World/World.h"
#include "../../Headers/World/GameObject.h"
#include "../../Headers/Resource/Resources.h"
#include <algorithm>
#include "../../Headers/Helper/StringUtils.h"
#include "../../Headers/World/Camera.h"

using namespace osomi;

Camera *LinkedObject::c = nullptr;

GameObject *World::add(std::string name, Shader *s, RenderObject *render, Texture *texture, vec3 position, vec3 rotation, vec3 scale){
	if (scale == vec3() || render == nullptr)return nullptr;
	
	for (u32 i = 0; i < gos.size(); i++)
		if (gos[i].go->getName() == name){
			printf("GameObjects need unique names!\n");
			return nullptr;
		}
	
	GameObject *go = new GameObject(this, name, render, texture, position, rotation, scale);
	gos.push_back(LinkedObject(go, s));
	return go;
}
bool World::remove(std::string name){
	for (u32 i = 0; i < gos.size(); i++)
		if (gos[i].go->getName() == name) {
			delete gos[i].go;
			gos.erase(gos.begin() + i);
			return true;
		}
	return false;
}
GameObject *World::get(std::string name){
	for (u32 i = 0; i < gos.size(); i++)
		if (gos[i].go->getName() == name) 
			return gos[i].go;
	return nullptr;
}
World::~World(){
	for (u32 i = 0; i < gos.size(); i++)
		delete gos[i].go;
	gos.clear();
}

void World::update(double delta, InputHandler *ih){
	for (u32 i = gos.size() - 1; i != 0xFFFFFFFF; i--) {
		if (gos[i].go->isDead()) {
			delete gos[i].go;
			gos.erase(gos.begin() + i);
			continue;
		}
		gos[i].go->update(delta, ih);
	}
}
void World::render(Camera *c, std::vector<Light> lights){
	Frustum f = c->toFrustum();
	
	LinkedObject::c = c;
	std::vector<LinkedObject> temp = gos;
	std::sort(temp.begin(), temp.end());
	Shader *prev = nullptr;
	RenderObject *pro = nullptr;
	Texture *t = nullptr;
	u32 rendered = 0, binds = 0, unbinds = 0, textureBinds = 0, shaderSwaps = 0;
	for (u32 i = 0; i < temp.size(); i++) {
		if (!f.intersection(temp[i].go->getAABB(), 15) || temp[i].go->isHidden())continue;
		if (prev != temp[i].s) {
			prev = temp[i].s;
			if(prev != nullptr){ 
				prev->use();
				prev->set("V", c->getView());
				prev->set("P", c->getProjection());
				prev->set("eye", c->getPosition());
				prev->set("actualLights", (i32)lights.size());
				for (u32 i = 0; i < lights.size(); i++) {
					prev->set(StringUtils::numstring(i, "lights[", "].position"), lights[i].getPosition());
					prev->set(StringUtils::numstring(i, "lights[", "].color"), lights[i].getColor());
					prev->set(StringUtils::numstring(i, "lights[", "].power"), lights[i].getPower());
				}
				shaderSwaps++;
			}
		}
		if (pro != temp[i].go->ro){
			if (pro != nullptr) {
				pro->unbind(prev);
				unbinds++;
			}
			pro = temp[i].go->ro;
			if (pro != nullptr) {
				pro->bind(prev);
				binds++;
			}
		}
		if (t != temp[i].go->getT()){
			t = temp[i].go->getT();
			if (t != nullptr)t->bind(GL_TEXTURE_2D);
			else glBindTexture(GL_TEXTURE_2D, 0);
			textureBinds++;
		}
		temp[i].go->nobindRender(prev, c, lights);
		rendered++;
	}
	if (pro != nullptr)pro->unbind(prev);
	if (t != nullptr)t->unbind(GL_TEXTURE_2D);
	printf("%u rendered, %u bound, %u unbound, %u textures bound, %u shaders swapped\n", rendered, binds, unbinds+1, textureBinds, shaderSwaps);
	int debug = 0;
}
//Sorts on:
//#1 Shader
//#2 Model
//#3 Texture
//#4 Distance to camera
float LinkedObject::distance(vec3 p){
	if (c == nullptr)return 0;
	return (p - c->getPosition()).length();
}
bool LinkedObject::operator<(const LinkedObject &lo){
	return s < lo.s || ((s == lo.s && go->getRO() < lo.go->getRO()) || ((s == lo.s && go->getRO() == lo.go->getRO() && go->getT() < lo.go->getT()) || (s == lo.s && lo.go->getRO() == lo.go->getRO() && go->getT() == lo.go->getT() && distance(go->getAABB().getCenter()) < distance(lo.go->getAABB().getCenter()))));
}