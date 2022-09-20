
typedef struct Color
{
	int r;
	int g;
	int b;
	int a;
} Color;

typedef struct Vector2
{
	float x;
	float y;
} Vector2;

typedef struct RigidBody
{
	Color color;
	Vector2 position;
	Vector2 scale;
	Vector2 velocity;
} RigidBody;
