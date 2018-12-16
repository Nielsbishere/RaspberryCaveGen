#pragma once
#include "../Math/MathDefines.h"

namespace osomi {
	class Light{
	private:
		vec3 position;
		vec3 color;
		float power;
	public:
		Light(vec3 pos, vec3 col, float pow): position(pos), color(col), power(pow){}
		vec3 getPosition(){ return position; }
		vec3 getColor(){ return color; }
		float getPower(){ return power; }
		void setPosition(vec3 pos){ position = pos; }
		void setColor(vec3 col){ color = col; }
		void setPower(float pow){ power = pow; }
		void applyMovement(vec3 vec){ setPosition(getPosition() + vec); }
	};
}