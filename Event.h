#pragma once


class Event {
public:
	enum Type {
		EAttachBomb,
		EExplosion,
		EDetonate,
		EDestroyedByPlayer
	};

	Type mType;

};
