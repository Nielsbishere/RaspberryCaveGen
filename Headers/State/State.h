#pragma once
class State{
public:
	virtual ~State(){}
	
	//For things that have to be drawn on the screen; after update.
	virtual void render() = 0;
	
	//For things that have to be done before rendering; like moving objects
	virtual void update(double delta) = 0;
	
	//For things that have to be done after rendering and updating; like updating the input handler, handling screenshots, etc.
	virtual void postRender() = 0;
};